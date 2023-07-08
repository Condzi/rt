/**
 * The math module is, at the moment, 99% copy-paste from previous projects.
 */

#include "vec.hxx"
#include "mat.hxx"

namespace rt {
[[nodiscard]] bool
f32_compare(f32 a, f32 b) {
  return (std::abs(a - b) < FLT_EPSILON);
}
} // namespace rt