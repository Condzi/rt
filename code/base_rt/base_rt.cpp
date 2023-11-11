#include "camera.cxx"
#include "materials.cxx"
#include "bvh.cxx"
#include "shapes.cxx"
#include "world.cxx"

namespace rt {
[[nodiscard]] Ray
make_ray(Vec3 origin, Vec3 direction) {
  total_ray_count++;
  return {.origin        = origin,
          .direction     = direction};
}

[[nodiscard]] Vec3
ray_at(Ray const &r, f32 t) {
  return r.origin + r.direction * t;
}
} // namespace rt
