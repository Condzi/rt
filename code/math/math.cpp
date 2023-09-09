#include "vec.cxx"
#include "aabb.cxx"
#include "mat.cxx"
#include "random.cxx"

namespace rt {
[[nodiscard]] bool
f32_compare(f32 a, f32 b) {
  return (std::abs(a - b) < FLT_EPSILON);
}

[[nodiscard]] f32
clamp(f32 x, f32 min, f32 max) {
  if (x < min) return min;
  if (x > max) return max;

  return x;
}

[[nodiscard]] Vec3
clamp_vec3(Vec3 x, f32 min, f32 max) {
  x.x = rt::clamp(x.x, min, max);
  x.y = rt::clamp(x.y, min, max);
  x.z = rt::clamp(x.z, min, max);

  return x;
}

[[nodiscard]] f32
fmin(f32 a, f32 b) {
  if (a < b) return a;

  return b;
}

[[nodiscard]] f32
fmax(f32 a, f32 b) {
  if (a < b) return b;
  
  return a;
}
} // namespace rt
