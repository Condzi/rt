namespace rt {
struct Hit_Info {
  Material_ID mat_id;
  Vec3        p;
  Vec3        normal;
  f32         t;
  bool        front_face;
};

// 12.4s for 10-10 params
[[nodiscard]] __forceinline bool
hit_sphere(Ray const &r, Sphere const &s, f32 t_min, f32 t_max, Hit_Info &hi) {
  Vec3 oc = r.origin - s.center;

  // Inline len_sq and dot
  // Hot \/\/
  f32 a = r.direction.x * r.direction.x + r.direction.y * r.direction.y +
          r.direction.z * r.direction.z;
  f32 half_b = oc.x * r.direction.x + oc.y * r.direction.y + oc.z * r.direction.z;
  f32 c      = oc.x * oc.x + oc.y * oc.y + oc.z * oc.z - s.radius * s.radius;
  f32 discriminant = half_b * half_b - a * c;
  // ^^ hot

  f32 sd = ::sqrtf(discriminant);

  // Optimized root calculation
  f32 root1 = (-half_b - sd) / a;
  f32 root2 = (-half_b + sd) / a;
  f32 root  = (root1 < t_max && root1 > t_min)   ? root1
              : (root2 < t_max && root2 > t_min) ? root2
                                                 : -1;

  if (root == -1) return false;

  hi.t = root;
  hi.p = ray_at(r, root);

  f32  inv_radius     = 1 / s.radius;
  Vec3 outward_normal = (hi.p - s.center) * inv_radius;

  hi.front_face = (dot(r.direction, outward_normal) < 0);
  hi.normal     = outward_normal * (hi.front_face ? 1.f : -1.f);
  hi.mat_id     = s.mat_id;

  return true;
}

// 1. finding the plane that contains that quad,
// 2. solving for the intersection of a ray and the quad-containing plane,
// 3. determining if the hit point lies inside the quad.
[[nodiscard]] bool
hit_quad(Ray const &r, Quad const &q, f32 t_min, f32 t_max, Hit_Info &hi) {
  f32 const denom = dot(q.normal, r.direction);

  // Ray is parallel to the plane
  if (::fabs(denom) < 1e-8) {
    return false;
  }

  // Intersection point is outside the ray interval
  f32 const t = (q.D - dot(q.normal, r.origin)) / denom;
  if (t < t_min || t > t_max) {
    return false;
  }

  Vec3 const intersection_point = ray_at(r, t);
  // Determine the hit point lies within the planar shape using its plane coordinates.

  Vec3 const planar_hitpt_vector = intersection_point - q.Q;
  f32 const  alpha               = dot(q.w, cross(planar_hitpt_vector, q.v));
  f32 const  beta                = dot(q.w, cross(q.u, planar_hitpt_vector));

  // Check if we're inside the (0, 1) range
  if ((alpha < 0 || alpha > 1) || (beta < 0 || beta > 1)) {
    return false;
  }

  // Ray hits the 2D shape; set the hit record and return true.
  hi.t          = t;
  hi.p          = intersection_point;
  hi.mat_id     = q.mat_id;
  hi.front_face = (dot(r.direction, q.normal) < 0);
  hi.normal     = q.normal * (hi.front_face ? 1.f : -1.f);

  return true;
}

// Returns true if there was a contact with at least one object
//
[[nodiscard]] bool
check_possible_contacts_for_collision(World const           &world,
                                      std::vector<Object_ID> contacts,
                                      Ray const             &r,
                                      Vec2                   t,
                                      Hit_Info              &hi) {
  // Sort by object type in order to help the CPU prefetcher
  //
  std::sort(contacts.begin(),
            contacts.end(),
            [](Object_ID const &a, Object_ID const &b) { return a.type < b.type; });

  bool any_hit = false;
  for (auto const &obj : contacts) {
    bool hit = false;
    switch (obj.type) {
      case ObjectType_Sphere: {
        Sphere const &s = world.spheres[obj.idx];
        hit             = hit_sphere(r, s, t.min, t.max, hi);
      } break;

      case ObjectType_Quad: {
        Quad const &q = world.quads[obj.idx];
        hit           = hit_quad(r, q, t.min, t.max, hi);
      } break;

      default: {
        // Unknown object!
        assert(false);
      }
    }

    if (hit) {
      any_hit = true;
      // We need to update how far the ray reaches.
      //
      t.max = hi.t;
    }
  }

  return any_hit;
}

// @Hot
[[nodiscard]] Vec3
ray_color(World const                 &world,
          Ray const                   &starting_ray,
          std::vector<BVH_Flat> const &bvh,
          s32                          depth) {
  Vec3 const background_color {0.5, 0.7, 1.0};
  Vec3       final_color = Vec3 {1, 1, 1};

  Ray current_ray = starting_ray;
  for (s32 i = 0; i < depth; i++) {
    // Broad-phase collision detection
    //
    std::vector<Object_ID> contacts = hit_BVH(bvh, current_ray);

    // Narrow-phase
    //
    Hit_Info hi;
    if (!check_possible_contacts_for_collision(
            world, contacts, current_ray, {.min = 0.001f, .max = FLT_MAX}, hi)) {
      return final_color * background_color;
    }

    Material const &material = world.materials[hi.mat_id];
    Vec3 const      emission = emit(material);

    Ray  scattered_ray;
    Vec3 attenuated_color;
    if (!scatter(material, current_ray, hi, attenuated_color, scattered_ray)) {
      return final_color * emission;
    }

    current_ray = scattered_ray;
    final_color = (final_color * attenuated_color) + emission;
  }

  return Vec3 {0, 0, 0};
}

s32 constexpr static NUM_CHANNELS = 4;
// @todo: move to ui
s32 const SAMPLES_PER_PIXEL = 50; // 500
// @todo: move to ui
s32 const MAX_DEPTH = 50; // 50

f32 const COLOR_SCALE = 1.0f / SAMPLES_PER_PIXEL;

std::atomic_int current_row{0};

s32
get_next_row(s32 max_row) {
  if (current_row + 1 >= max_row) return -1;
  current_row += 1;
  return current_row;
}

void
rt_loop_balanced(s32                          image_width,
                 s32                          image_height,
                 u8                          *buffer,
                 std::vector<BVH_Flat> const &bvh,
                 Camera const                &cam,
                 std::atomic_bool            &thread_flag,
                 World const                 &world) {

  for (s32 j = get_next_row(image_height); j != -1; j = get_next_row(image_height)) {
    for (s32 i = 0; i < image_width; ++i) {
      Vec3 pixel_color = {0, 0, 0};
      for (s32 k = 0; k < SAMPLES_PER_PIXEL; k++) {
        auto u = (f32(i) + random_f32()) / (image_width - 1);
        auto v = (f32(j) + random_f32()) / (image_height - 1);

        Ray r       = get_ray_at(cam, u, v);
        pixel_color += ray_color(world, r, bvh, MAX_DEPTH);
      }

      pixel_color = pixel_color * COLOR_SCALE;
      pixel_color = Vec3 {.r = ::sqrt(pixel_color.r),
                          .g = ::sqrt(pixel_color.g),
                          .b = ::sqrt(pixel_color.b)};
      pixel_color = clamp_vec3(pixel_color, 0.0f, 0.999f);

      size_t const base_idx = NUM_CHANNELS * (j * image_width + i);
      buffer[base_idx + 0]  = u8(pixel_color.r * 255);
      buffer[base_idx + 1]  = u8(pixel_color.g * 255);
      buffer[base_idx + 2]  = u8(pixel_color.b * 255);
      buffer[base_idx + 3]  = 255;
    }
  }

  thread_flag = true;
}

[[nodiscard]] CPU_RT_Output
do_ray_tracing(CPU_RT_Input const &in) {
  s32 const image_width  = (s32)in.im_size.width;
  s32 const image_height = (s32)in.im_size.height;

  u8 *buffer = perm<u8>(image_width * image_height * NUM_CHANNELS);

  s32 const num_of_threads_supported = (s32)std::thread::hardware_concurrency();
  std::atomic_bool *thread_flags = new std::atomic_bool[num_of_threads_supported];

  for (s32 i = 0; i < num_of_threads_supported; i++) {
    std::thread([=] {
      rt_loop_balanced(
          image_width, image_height, buffer, in.bvh, in.c, thread_flags[i], in.w);
    }).detach();
  }

  // No need to wait because everything will be updated in realtime.

  return {.image_size   = {(f32)image_width, (f32)image_height},
          .rgba_data    = {.count = image_width * image_height * NUM_CHANNELS,
                           .bytes = buffer},
          .num_threads  = num_of_threads_supported,
          .thread_flags = thread_flags};
}
} // namespace rt

#include "cpu_materials.cxx"
