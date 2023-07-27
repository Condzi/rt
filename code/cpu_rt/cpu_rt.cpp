#include "camera.cxx"

namespace rt {
struct Hit_Info {
  Vec3 p;
  Vec3 normal;
  f32  t;
  bool front_face;
};

struct Sphere {
  Vec3 center;
  f32  radius;
};

struct World {
  Sphere *spheres;
  s32     num_spheres;
};

[[nodiscard]] Vec3
at(Ray const &r, f32 t) {
  return r.origin + r.direction * t;
}

[[nodiscard]] bool
hit_sphere(Ray const &r, Sphere const &s, f32 t_min, f32 t_max, Hit_Info &hi) {
  Vec3      oc           = r.origin - s.center;
  f32 const a            = len_sq(r.direction);
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

  return true;
}

[[nodiscard]] bool
hit_world(Ray const &r, World const &w, f32 t_min, f32 t_max, Hit_Info &hi) {
  Hit_Info hi_temp;
  bool     hit_anything = false;
  f32      closest      = t_max;

  for (s32 i = 0; i < w.num_spheres; i++) {
    if (hit_sphere(r, w.spheres[i], t_min, closest, hi_temp)) {
      hit_anything = true;
      closest      = hi_temp.t;
      hi           = hi_temp;
    }
  }

  return hit_anything;
}

[[nodiscard]] Vec3
ray_color(Ray const &r, World const &w, s32 depth) {
  if (depth == 0) {
    return Vec3 {0, 0, 0};
  }

  Hit_Info hi;
  // @Note: 0.001 instead 0 fixes shadow acne
  if (hit_world(r, w, 0.001f, FLT_MAX, hi)) {

    // 3 alternative diffuse formulas
#if 0
    Vec3 const target = hi.p + hi.normal + random_in_unit_sphere();
#elif 0
    Vec3 const target = hi.p + hi.normal + random_unit_vector();
#else
    Vec3 const target = hi.p + random_in_hemisphere(hi.normal);
#endif

    Ray const r2 = {hi.p, target - hi.p};
    return (ray_color(r2, w, depth - 1)) * 0.5f;
  }

  Vec3  dir    = normalized(r.direction);
  float t      = 0.5f * (dir.y + 1.0f);
  f32   scalar = 1 - t;

  Vec3 color = Vec3 {1, 1, 1} * scalar + Vec3 {0.5, 0.7, 1.0} * t;
  return color;
}

[[nodiscard]] Rt_Output
do_raytraycing() {
  f32 const aspect_ratio = 16.0 / 9;
  s32 const image_width  = 400;
  s32 const image_height = (s32)(image_width / aspect_ratio);

  // World
  World w;
  w.num_spheres = 2;
  w.spheres     = (Sphere *)alloc_perm(w.num_spheres * sizeof(*w.spheres));

  w.spheres[0] = Sphere {.center = {0, 0, -1}, .radius = 0.5f};
  w.spheres[1] = Sphere {.center = {0, -100.5f, -1}, .radius = 100.f};

  Camera cam = make_camera();

  // Render

  s32 constexpr static NUM_CHANNELS = 4;
  u8 *buffer = (u8 *)alloc_perm(image_width * image_height * NUM_CHANNELS);

  // @todo: move to ui
  s32 const SAMPLES_PER_PIXEL = 100;
  // @todo: move to ui
  s32 const MAX_DEPTH = 10;

  f32 const COLOR_SCALE = 1.0f / SAMPLES_PER_PIXEL;

  for (int j = image_height - 1; j >= 0; --j) {
    logf("Scanlines remaining: %d\n", j);
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

  return {.image_size = {(f32)image_width, (f32)image_height},
          .rgba_data  = {.count = image_width * image_height * 4, .bytes = buffer}};
}
} // namespace rt
