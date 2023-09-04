#include "camera.cxx"

namespace rt {
struct Material;

struct Hit_Info {
  Material *material;
  Vec3      p;
  Vec3      normal;
  f32       t;
  bool      front_face;
};

struct Material {
  // Returns true when the ray was absorbed.
  // Calculates the new color and ray.
  virtual bool
  scatter(Ray const &in, Hit_Info const &hi, Vec3 &attenuation_color, Ray &out) = 0;
};

struct Lambertian : Material {
  Vec3 albedo;

  Lambertian(Vec3 albedo_) : albedo(albedo_) {}

  [[nodiscard]] bool
  scatter(Ray const      &in,
          Hit_Info const &hi,
          Vec3           &attenuation_color,
          Ray            &out) override {
    (void)in;
#if 0 // Alternative formulas
    Vec3 scatter_direction = hi.normal + random_in_unit_sphere();
    Vec3 scatter_direction = random_in_hemisphere(hi.normal);
#endif
    Vec3 scatter_direction = hi.normal + random_unit_vector();
    if (near_zero(scatter_direction)) {
      scatter_direction = hi.normal;
    }

    out               = {.origin = hi.p, .direction = scatter_direction};
    attenuation_color = albedo;

    return true;
  }
};

[[nodiscard]] Vec3
reflect(Vec3 v, Vec3 n) {
  return v - n * 2 * dot(v, n);
}

struct Metal : Material {
  Vec3 albedo;
  f32  fuzz;

  Metal(Vec3 albedo_, f32 fuzz_) : albedo(albedo_), fuzz(fuzz_ < 1 ? fuzz_ : 1) {}

  [[nodiscard]] bool
  scatter(Ray const      &in,
          Hit_Info const &hi,
          Vec3           &attenuation_color,
          Ray            &out) override {
    Vec3 const reflected = reflect(normalized(in.direction), hi.normal);
    Vec3 const direction = reflected + random_in_unit_sphere() * fuzz;
    out                  = {.origin = hi.p, .direction = direction};

    attenuation_color = albedo;
    return (dot(out.direction, hi.normal) > 0);
  }
};

// from Snell's law
[[nodiscard]] Vec3
refract(Vec3 uv, Vec3 n, f32 etai_over_etat) {
  f32 const  cos_theta      = fmin(dot(uv * -1.f, n), 1.0);
  Vec3 const r_out_perp     = (uv + n * cos_theta) * etai_over_etat;
  Vec3 const r_out_parallel = n * (-::sqrtf(::fabsf(1.0f - len_sq(r_out_perp))));

  return r_out_perp + r_out_parallel;
}

// Schlick's approximation of reflactance
[[nodiscard]] f32
reflactance(f32 cosine, f32 refraction_index) {
  f32 r0 = (1 - refraction_index) / (1 + refraction_index);
  r0     = r0 * r0;

  return r0 + (1 - r0) * ::powf((1 - cosine), 5);
}

struct Dielectric : Material {
  f32 refraction_index;

  Dielectric(f32 refraction_index_) : refraction_index(refraction_index_) {}

  [[nodiscard]] bool
  scatter(Ray const      &in,
          Hit_Info const &hi,
          Vec3           &attenuation_color,
          Ray            &out) override {
    f32 const refraction_ratio =
        hi.front_face ? (1.0f / refraction_index) : (refraction_index);

    Vec3 const unit_direction = normalized(in.direction);
    f32 const  cos_theta      = fmin(dot(unit_direction * -1.f, hi.normal), 1.0);
    f32 const  sin_theta      = ::sqrt(1.0f - cos_theta * cos_theta);

    bool const can_refract = (refraction_ratio * sin_theta) <= 1.0f;
    bool const reflactance_test =
        reflactance(cos_theta, refraction_ratio) > random_f32();

    Vec3 direction;
    if (can_refract && !reflactance_test) {
      direction = refract(unit_direction, hi.normal, refraction_ratio);
    } else {
      direction = reflect(unit_direction, hi.normal);
    }

    out               = {.origin = hi.p, .direction = direction};
    attenuation_color = Vec3 {1.0f, 1.0f, 1.0f};

    return true;
  }
};

struct Sphere {
  Vec3      center;
  f32       radius; // @Note: can be negative: surface normals will point inward.
  Material *material;
};

struct World {
  Sphere *spheres;
  s32     num_spheres;
};

[[nodiscard]] Vec3
at(Ray const &r, f32 t) {
  return r.origin + r.direction * t;
}

// 12.5s
[[nodiscard]] bool
hit_sphere_v2(Ray const &r, Sphere const &s, f32 t_min, f32 t_max, Hit_Info &hi) {
  Vec3 oc = r.origin - s.center;

  // Inline len_sq and dot
  f32 a = r.direction.x * r.direction.x + r.direction.y * r.direction.y +
          r.direction.z * r.direction.z;
  f32 half_b = oc.x * r.direction.x + oc.y * r.direction.y + oc.z * r.direction.z;
  f32 c      = oc.x * oc.x + oc.y * oc.y + oc.z * oc.z - s.radius * s.radius;
  f32 discriminant = half_b * half_b - a * c;

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

  assert(hi.material);
  return true;
}

// 14s
[[nodiscard]] bool
hit_sphere(Ray const &r, Sphere const &s, f32 t_min, f32 t_max, Hit_Info &hi) {
  Vec3      oc           = r.origin - s.center;
  f32 const a = len_sq(r.direction); // dot(a, a), dot(a,b) = (a.x*b.x +...)
  f32 const half_b       = dot(oc, r.direction);
  f32 const c            = len_sq(oc) - s.radius * s.radius;
  f32 const discriminant = half_b * half_b - a * c;

  if (discriminant < 0) {
    return false;
  }

  f32 const sd   = ::sqrtf(discriminant);
  f32       root = (-half_b - sd) / a;

  if (root < t_min || root > t_max) {
    root = (-half_b + sd) / a;
    if (root < t_min || root > t_max) {
      return false;
    }
  }

  hi.t = root;
  hi.p = at(r, root);

  Vec3 const outward_normal = (hi.p - s.center) * (1 / s.radius);
  hi.front_face             = (dot(r.direction, outward_normal) < 0);
  hi.normal                 = outward_normal * (hi.front_face ? 1.f : -1.f);
  hi.material               = s.material;

  assert(hi.material);
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
ray_color(Ray const &r, World const &w, s32 depth) {
  if (depth == 0) {
    return Vec3 {0, 0, 0};
  }

  Hit_Info hi;
  // @Note: 0.001 instead 0 fixes shadow acne
  if (hit_world(r, w, 0.001f, FLT_MAX, hi)) {
    Ray  scattered;
    Vec3 attenuated_color;

    if (hi.material->scatter(r, hi, attenuated_color, scattered)) {
      return attenuated_color * ray_color(scattered, w, depth - 1);
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
s32 const SAMPLES_PER_PIXEL = 10; // 500
// @todo: move to ui
s32 const MAX_DEPTH = 10; // 50

f32 const COLOR_SCALE = 1.0f / SAMPLES_PER_PIXEL;

// called by std::thread
// no logging because concurrency stuff
void
rt_loop(s32               start_height,
        s32               end_height,
        s32               image_width,
        s32               image_height,
        u8               *buffer,
        World            &w,
        Camera           &cam,
        std::atomic_bool &thread_flag) {
  for (int j = end_height; j >= start_height; --j) {
    for (int i = 0; i < image_width; ++i) {
      Vec3 pixel_color = {0, 0, 0};
      for (s32 k = 0; k < SAMPLES_PER_PIXEL; k++) {
        auto u = (f32(i) + random_f32()) / (image_width - 1);
        auto v = (f32(j) + random_f32()) / (image_height - 1);

        Ray r       = get_ray_at(cam, u, v);
        pixel_color = pixel_color + ray_color(r, w, MAX_DEPTH);
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

  // Render

  u8 *buffer = (u8 *)alloc_perm(image_width * image_height * NUM_CHANNELS);

  s32 const num_of_threads_supported = (s32)std::thread::hardware_concurrency();
  // @Fixme: If the modulo is not 0, we will have leftover scanlines
  s32 const y_per_thread = image_height / num_of_threads_supported;
  logf("%d therads -- %d scanlines per thread\n",
       num_of_threads_supported,
       y_per_thread);

  std::atomic_bool *thread_flags = new std::atomic_bool[num_of_threads_supported];

  for (s32 i = 0; i < num_of_threads_supported; i++) {
    std::thread([=] {
      rt_loop(i * y_per_thread,
              (i + 1) * y_per_thread - 1,
              image_width,
              image_height,
              buffer,
              w,
              cam,
              thread_flags[i]);
    }).detach();
  }

  // No need to wait because everything will be updated in realtime.

  return {.image_size   = {(f32)image_width, (f32)image_height},
          .rgba_data    = {.count = image_width * image_height * 4, .bytes = buffer},
          .num_threads  = num_of_threads_supported,
          .thread_flags = thread_flags};
}

[[nodiscard]] World
random_scene() {
  World w;
  w.num_spheres = 22 * 22 + 2;
  w.spheres     = (Sphere *)alloc_perm(w.num_spheres * sizeof(Sphere));

  Lambertian *ground_material = (Lambertian *)alloc_perm(sizeof(Lambertian));
  new (ground_material) Lambertian {Vec3 {0.5, 0.5, 0.5}};
  w.spheres[0] = Sphere {Vec3 {0, -1000, 0}, 1000, ground_material};

  s32 sphere_idx        = 1;
  s32 iteration_counter = 0;

  for (s32 a = -11; a < 11; a++) {
    for (s32 b = -11; b < 11; b++) {
      iteration_counter++;
      // Space for 3 giant spheres, added outside the loop
      if (w.num_spheres - sphere_idx == 3) {
        continue;
      }

      Vec3 const center {a + 0.9f * random_f32(), 0.2f, b + 0.9f * random_f32()};

      if (dist(center, Vec3 {4, 0.2f, 0}) < 0.9f) {
        continue;
      }

      f32 const choose_mat = random_f32();
      if (choose_mat < 0.8f) {
        Vec3 const  albedo = random_vec3() * random_vec3();
        Lambertian *mat    = (Lambertian *)alloc_perm(sizeof(Lambertian));
        new (mat) Lambertian {albedo};
        w.spheres[sphere_idx] = Sphere {center, 0.2f, mat};
      } else if (choose_mat < 0.95f) {
        Vec3 const albedo = random_vec3_in_range(0.5f, 1);
        f32 const  fuzz   = random_f32_in_range(0, 0.5f);
        Metal     *mat    = (Metal *)alloc_perm(sizeof(Metal));
        new (mat) Metal {albedo, fuzz};
        w.spheres[sphere_idx] = Sphere {center, 0.2f, mat};
      } else {
        Dielectric *mat = (Dielectric *)alloc_perm(sizeof(Dielectric));
        new (mat) Dielectric {1.5f};
        w.spheres[sphere_idx] = Sphere {center, 0.2f, mat};
      }

      sphere_idx++;
    }
  }
  logf("%d iterations \n", iteration_counter);

  Dielectric *mat1 = (Dielectric *)alloc_perm(sizeof(Dielectric));
  new (mat1) Dielectric {1.5f};

  Lambertian *mat2 = (Lambertian *)alloc_perm(sizeof(Lambertian));
  new (mat2) Lambertian {Vec3 {0.4f, 0.2f, 0.1f}};

  Metal *mat3 = (Metal *)alloc_perm(sizeof(Metal));
  new (mat3) Metal {Vec3 {0.7f, 0.6f, 0.5f}, 0.0f};

  w.spheres[sphere_idx] = Sphere {Vec3 {0, 1, 0}, 1.0, mat1};
  sphere_idx++;
  w.spheres[sphere_idx] = Sphere {Vec3 {-4, 1, 0}, 1.0, mat2};
  sphere_idx++;
  w.spheres[sphere_idx] = Sphere {Vec3 {4, 1, 0}, 1.0, mat3};
  sphere_idx++;

  logf("%d/%d spheres generated.\n", sphere_idx, w.num_spheres);
  assert(sphere_idx <= w.num_spheres);
  w.num_spheres = sphere_idx;

  return w;
}
} // namespace rt
