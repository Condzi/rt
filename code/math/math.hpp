/**
 * The math module is, at the moment, 99% copy-paste from previous projects.
 */

#include "vec.hxx"
#include "mat.hxx"
#include "random.hxx"

namespace rt {
[[nodiscard]] bool
f32_compare(f32 a, f32 b);

[[nodiscard]] f32
clamp(f32 x, f32 min, f32 max);

[[nodiscard]] Vec3
clamp_vec3(Vec3 x, f32 min, f32 max);
} // namespace rt
