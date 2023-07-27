#include "vec.cxx"
#include "mat.cxx"
#include "random.cxx"

namespace rt {
[[nodiscard]] bool
f32_compare(f32 a, f32 b) {
  return (std::abs(a - b) < FLT_EPSILON);
}
} // namespace rt
