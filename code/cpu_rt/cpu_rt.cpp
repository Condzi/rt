namespace rt {
struct Hit_Info {
  Material *material;
  Vec3      p;
  Vec3      normal;
  f32       t;
  bool      front_face;
};

[[nodiscard]] Sphere
make_sphere(Vec3 center, f32 r) {
  Vec3 const radius_vec {.x = r, .y = r, .z = r};
  Vec3 const aabb_min = center - radius_vec;
  Vec3 const aabb_max = center + radius_vec;

  return {.center   = center,
          .radius   = r,
          .aabb     = make_aabb_from_extremas(aabb_min, aabb_max),
          .material = NULL};
}

struct World {
  Sphere    *spheres;
  s32        num_spheres_reserved;
  s32        num_spheres;
  AABB       aabb;
};

void
add_sphere(World &w, Sphere s, Material *mat) {
  assert(w.num_spheres < w.num_spheres_reserved);

  s32 const idx           = w.num_spheres++;
  w.spheres[idx]          = s;
  w.spheres[idx].material = mat;
  w.aabb                  = make_aabb_from_aabbs(w.aabb, s.aabb);
}

[[nodiscard]] Vec3
at(Ray const &r, f32 t) {
  return r.origin + r.direction * t;
}

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

  if (discriminant < 0) {
    return false;
  }

  f32 sd = ::sqrtf(discriminant);

  // Optimized root calculation
  f32 root1 = (-half_b - sd) / a;
  f32 root2 = (-half_b + sd) / a;
  f32 root  = (root1 < t_max && root1 > t_min)   ? root1
              : (root2 < t_max && root2 > t_min) ? root2
                                                 : -1;

  if (root == -1) return false;

  hi.t = root;
  hi.p = at(r, root);

  f32  inv_radius     = 1 / s.radius;
  Vec3 outward_normal = (hi.p - s.center) * inv_radius;

  hi.front_face = (dot(r.direction, outward_normal) < 0);
  hi.normal     = outward_normal * (hi.front_face ? 1.f : -1.f);
  hi.material   = s.material;

  return true;
}

[[nodiscard]] bool
hit_world(Ray const &r, World const &w, f32 t_min, f32 t_max, Hit_Info &hi) {
  Hit_Info hi_temp;
  bool     hit_anything = false;
  f32      closest      = t_max;

  s32 hit_idx = -1;
  for (s32 i = 0; i < w.num_spheres; i++) {
    // @Hot
    if (hit_sphere(r, w.spheres[i], t_min, closest, hi_temp)) {
      hit_anything = true;
      closest      = hi_temp.t;
      hi           = hi_temp;
      hit_idx      = i;
    }
  }

  return hit_anything;
}

// @Hot
[[nodiscard]] Vec3
// ray_color(Ray const &r, World const &w, s32 depth) {
ray_color(Ray const &r, BVH_Node *const root, s32 depth) {
  if (depth == 0) {
    return Vec3 {0, 0, 0};
  }

  Hit_Info hi;
  // @Note: 0.001 instead 0 fixes shadow acne
  // if (hit_world(r, w, 0.001f, FLT_MAX, hi)) {
  if (hit_BVH(root, r, {.min = 0.001f, .max = FLT_MAX}, hi)) {
    Ray  scattered;
    Vec3 attenuated_color;

    if (hi.material->scatter(r, hi, attenuated_color, scattered)) {
      return attenuated_color * ray_color(scattered, root, depth - 1);
    } else {
      return Vec3 {0, 0, 0};
    }
  }

  Vec3  dir    = normalized(r.direction);
  float t      = 0.5f * (dir.y + 1.0f);
  f32   scalar = 1 - t;

  Vec3 color = Vec3 {1, 1, 1} * scalar + Vec3 {0.5, 0.7, 1.0} * t;
  return color;
}

[[nodiscard]] World
random_scene();

s32 constexpr static NUM_CHANNELS = 4;
// @todo: move to ui
s32 const SAMPLES_PER_PIXEL = 500; // 500
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
rt_loop_balanced(s32               max_row,
                 s32               image_width,
                 s32               image_height,
                 u8               *buffer,
                 BVH_Node         *bvh_root,
                 Camera           &cam,
                 std::atomic_bool &thread_flag) {
  while (true) {
    s32 j = get_next_row(max_row);
    if (j == -1) break;

    for (int i = 0; i < image_width; ++i) {
      Vec3 pixel_color = {0, 0, 0};
      for (s32 k = 0; k < SAMPLES_PER_PIXEL; k++) {
        auto u = (f32(i) + random_f32()) / (image_width - 1);
        auto v = (f32(j) + random_f32()) / (image_height - 1);

        Ray r       = get_ray_at(cam, u, v);
        pixel_color = pixel_color + ray_color(r, bvh_root, MAX_DEPTH);
      }

      pixel_color = pixel_color * COLOR_SCALE;
      pixel_color = Vec3 {.r = sqrt(pixel_color.r),
                          .g = sqrt(pixel_color.g),
                          .b = sqrt(pixel_color.b)};
      pixel_color = clamp_vec3(pixel_color, 0.0f, 0.999f);

      buffer[NUM_CHANNELS * (j * image_width + i)]     = u8(pixel_color.r * 255);
      buffer[NUM_CHANNELS * (j * image_width + i) + 1] = u8(pixel_color.g * 255);
      buffer[NUM_CHANNELS * (j * image_width + i) + 2] = u8(pixel_color.b * 255);
      buffer[NUM_CHANNELS * (j * image_width + i) + 3] = 255;
    }
  }

  thread_flag = true;
}

[[nodiscard]] Rt_Output
do_raytraycing() {
  f32 const aspect_ratio = 3 / 2.f;
  s32 const image_width  = 1000;
  s32 const image_height = (s32)(image_width / aspect_ratio);

  // Camera setup
  Vec3 const lookfrom {13, 2, 3};
  Vec3 const lookat {0, 0, 0};
  Vec3 const vup {0, 1, 0};
  f32 const  dist_to_focus = 10.0f;
  f32 const  aperture      = 0.1f;

  // Static so it doesnt go out of stack
  static Camera cam = make_camera(
      lookfrom, lookat, vup, 20.0f, aspect_ratio, aperture, dist_to_focus);

  // World
  // Static so it doesnt go out of stack
  static World w = random_scene();
  BVH_Node    *bvh_root = make_BVH(w.spheres, 0, w.num_spheres, w.aabb);

  // Render

  u8 *buffer = (u8 *)alloc_perm(image_width * image_height * NUM_CHANNELS);

  s32 const num_of_threads_supported = (s32)std::thread::hardware_concurrency();

  std::atomic_bool *thread_flags = new std::atomic_bool[num_of_threads_supported];

  for (s32 i = 0; i < num_of_threads_supported; i++) {
    std::thread([=] {
      rt_loop_balanced(image_height,
                       image_width,
                       image_height,
                       buffer,
                       bvh_root,
                       cam,
                       thread_flags[i]);
    }).detach();
  }

  // No need to wait because everything will be updated in realtime.

  return {.image_size   = {(f32)image_width, (f32)image_height},
          .rgba_data    = {.count = image_width * image_height * NUM_CHANNELS,
                           .bytes = buffer},
          .num_threads  = num_of_threads_supported,
          .thread_flags = thread_flags};
}

[[nodiscard]] World
random_scene() {
  World w;
  w.num_spheres          = 0;
  w.aabb                 = AABB {};
  w.num_spheres_reserved = 22 * 22 + 2;
  w.spheres = (Sphere *)alloc_perm(w.num_spheres_reserved * sizeof(Sphere));

  Lambertian *ground_material = (Lambertian *)alloc_perm(sizeof(Lambertian));
  new (ground_material) Lambertian {Vec3 {0.5, 0.5, 0.5}};
  add_sphere(w, make_sphere(Vec3 {0, -1000, 0}, 1000), ground_material);

  s32 iteration_counter = 0;

  for (s32 a = -11; a < 11; a++) {
    for (s32 b = -11; b < 11; b++) {
      iteration_counter++;
      // Space for 3 giant spheres, added outside the loop
      if (w.num_spheres_reserved - w.num_spheres == 3) {
        continue;
      }

      Vec3 const center {a + 0.9f * random_f32(), 0.2f, b + 0.9f * random_f32()};

      if (dist(center, Vec3 {4, 0.2f, 0}) < 0.9f) {
        continue;
      }

      f32 const choose_mat = random_f32();
      Material *mat;
      if (choose_mat < 0.8f) {
        Vec3 const  albedo = random_vec3() * random_vec3();
        mat                = (Lambertian *)alloc_perm(sizeof(Lambertian));
        new (mat) Lambertian {albedo};
      } else if (choose_mat < 0.95f) {
        Vec3 const albedo = random_vec3_in_range(0.5f, 1);
        f32 const  fuzz   = random_f32_in_range(0, 0.5f);
        mat               = (Metal *)alloc_perm(sizeof(Metal));
        new (mat) Metal {albedo, fuzz};
      } else {
        mat = (Dielectric *)alloc_perm(sizeof(Dielectric));
        new (mat) Dielectric {1.5f};
      }

      add_sphere(w, make_sphere(center, 0.2f), mat);
    }
  }
  logf("%d iterations \n", iteration_counter);

  Dielectric *mat1 = (Dielectric *)alloc_perm(sizeof(Dielectric));
  new (mat1) Dielectric {1.5f};

  Lambertian *mat2 = (Lambertian *)alloc_perm(sizeof(Lambertian));
  new (mat2) Lambertian {Vec3 {0.4f, 0.2f, 0.1f}};

  Metal *mat3 = (Metal *)alloc_perm(sizeof(Metal));
  new (mat3) Metal {Vec3 {0.7f, 0.6f, 0.5f}, 0.0f};

  add_sphere(w, make_sphere(Vec3 {0, 1, 0}, 1.0), mat1);
  add_sphere(w, make_sphere(Vec3 {-4, 1, 0}, 1.0), mat2);
  add_sphere(w, make_sphere(Vec3 {4, 1, 0}, 1.0), mat3);

  logf("%d/%d spheres generated.\n", w.num_spheres, w.num_spheres_reserved);

  return w;
}
} // namespace rt

#include "camera.cxx"
#include "materials.cxx"
#include "bvh.cxx"
