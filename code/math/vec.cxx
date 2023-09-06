/**
 * Functions
 */

namespace rt {
[[nodiscard]] f32
dist_sq(Vec2 a, Vec2 b) {
  b = b - a;
  return dot(b, b);
}

[[nodiscard]] f32
dist_sq(Vec3 a, Vec3 b) {
  b = b - a;
  return dot(b, b);
}

[[nodiscard]] f32
dist_sq(Vec4 a, Vec4 b) {
  b = b - a;
  return dot(b, b);
}

[[nodiscard]] f32
dist(Vec2 a, Vec2 b) {
  return std::sqrt(dist_sq(a, b));
}

[[nodiscard]] f32
dist(Vec3 a, Vec3 b) {
  return std::sqrt(dist_sq(a, b));
}

[[nodiscard]] f32
dist(Vec4 a, Vec4 b) {
  return std::sqrt(dist_sq(a, b));
}

[[nodiscard]] f32
len_sq(Vec2 v) {
  return dot(v, v);
}

[[nodiscard]] f32
len_sq(Vec3 const &v) {
  return dot(v, v);
}

[[nodiscard]] f32
len_sq(Vec4 const &v) {
  return dot(v, v);
}

[[nodiscard]] f32
len(Vec2 v) {
  return std::sqrt(len_sq(v));
}

[[nodiscard]] f32
len(Vec3 const &v) {
  return std::sqrt(len_sq(v));
}

[[nodiscard]] f32
len(Vec4 const &v) {
  return std::sqrt(len_sq(v));
}

Vec2 &
normalize(Vec2 &v) {
  dbg_check_(!f32_compare(0, len(v)));
  return (v *= 1 / len(v));
}

Vec3 &
normalize(Vec3 &v) {
  dbg_check_(!f32_compare(0, len(v)));
  return (v *= 1 / len(v));
}

Vec4 &
normalize(Vec4 &v) {
  dbg_check_(!f32_compare(0, len(v)));
  return (v *= 1 / len(v));
}

[[nodiscard]] Vec2
normalized(Vec2 v) {
  return normalize(v);
}

[[nodiscard]] Vec3
normalized(Vec3 v) {
  return normalize(v);
}

[[nodiscard]] Vec4
normalized(Vec4 v) {
  return normalize(v);
}

[[nodiscard]] f32
dot(Vec2 a, Vec2 b) {
  a *= b;
  return (a.x + a.y);
}

[[nodiscard]] f32
dot(Vec3 a, Vec3 b) {
  a *= b;
  return (a.x + a.y + a.z);
}

[[nodiscard]] f32
dot(Vec4 a, Vec4 b) {
  a *= b;
  return (a.x + a.y + a.z + a.w);
}

[[nodiscard]] Vec3
cross(Vec3 a, Vec3 b) {
  Vec3 c;
  c.x = a.y * b.z - a.z * b.y;
  c.y = a.z * b.x - a.x * b.z;
  c.z = a.x * b.y - a.y * b.x;

  return c;
}

[[nodiscard]] bool
near_zero(Vec2 v) {
  return f32_compare(v.x, 0) && f32_compare(v.y, 0);
}

[[nodiscard]] bool
near_zero(Vec3 v) {
  return f32_compare(v.x, 0) && f32_compare(v.y, 0) && f32_compare(v.z, 0);
}

/**
 * Operators
 */
[[nodiscard]] bool
operator==(Vec2 a, Vec2 b) {
  return (mem_comp_(&a, &b, sizeof(a)) == 0);
}

[[nodiscard]] Vec2
operator+(Vec2 a, Vec2 b) {
  a.x += b.x;
  a.y += b.y;
  return a;
}

[[nodiscard]] Vec2
operator-(Vec2 a, Vec2 b) {
  a.x -= b.x;
  a.y -= b.y;
  return a;
}

[[nodiscard]] Vec2
operator*(Vec2 a, Vec2 b) {
  a.x *= b.x;
  a.y *= b.y;
  return a;
}

[[nodiscard]] Vec2
operator/(Vec2 a, Vec2 b) {
  dbg_check_(!f32_compare(b.x, 0) && !f32_compare(b.y, 0));
  a.x /= b.x;
  a.y /= b.y;
  return a;
}

[[nodiscard]] Vec2
operator*(Vec2 v, f32 x) {
  v.x *= x;
  v.y *= x;
  return v;
}

Vec2 &
operator+=(Vec2 &a, Vec2 b) {
  a = a + b;
  return a;
}

Vec2 &
operator-=(Vec2 &a, Vec2 b) {
  a = a - b;
  return a;
}

Vec2 &
operator*=(Vec2 &a, Vec2 b) {
  a = a * b;
  return a;
}

Vec2 &
operator/=(Vec2 &a, Vec2 b) {
  a = a / b;
  return a;
}

Vec2 &
operator*=(Vec2 &a, f32 x) {
  a = a * x;
  return a;
}

[[nodiscard]] bool
operator==(Vec3 a, Vec3 b) {
  return (mem_comp_(&a, &b, sizeof(a)) == 0);
}

[[nodiscard]] Vec3
operator+(Vec3 a, Vec3 b) {
  a.x += b.x;
  a.y += b.y;
  a.z += b.z;
  return a;
}

[[nodiscard]] Vec3
operator-(Vec3 a, Vec3 b) {
  a.x -= b.x;
  a.y -= b.y;
  a.z -= b.z;
  return a;
}

[[nodiscard]] Vec3
operator*(Vec3 a, Vec3 b) {
  a.x *= b.x;
  a.y *= b.y;
  a.z *= b.z;
  return a;
}

[[nodiscard]] Vec3
operator/(Vec3 a, Vec3 b) {
  dbg_check_(!f32_compare(b.x, 0));
  dbg_check_(!f32_compare(b.y, 0));
  dbg_check_(!f32_compare(b.z, 0));

  a.x /= b.x;
  a.y /= b.y;
  a.z /= b.z;
  return a;
}

[[nodiscard]] Vec3
operator*(Vec3 v, f32 x) {
  v.x *= x;
  v.y *= x;
  v.z *= x;
  return v;
}

Vec3 &
operator+=(Vec3 &a, Vec3 b) {
  a = a + b;
  return a;
}

Vec3 &
operator-=(Vec3 &a, Vec3 b) {
  a = a - b;
  return a;
}

Vec3 &
operator*=(Vec3 &a, Vec3 b) {
  a = a * b;
  return a;
}

Vec3 &
operator/=(Vec3 &a, Vec3 b) {
  a = a / b;
  return a;
}

Vec3 &
operator*=(Vec3 &a, f32 x) {
  a = a * x;
  return a;
}

[[nodiscard]] bool
operator==(Vec4 a, Vec4 b) {
  return (mem_comp_(&a, &b, sizeof(a)) == 0);
}

[[nodiscard]] Vec4
operator+(Vec4 a, Vec4 b) {
  a.x += b.x;
  a.y += b.y;
  a.z += b.z;
  a.w += b.w;
  return a;
}

[[nodiscard]] Vec4
operator-(Vec4 a, Vec4 b) {
  a.x -= b.x;
  a.y -= b.y;
  a.z -= b.z;
  a.w -= b.w;
  return a;
}

[[nodiscard]] Vec4
operator*(Vec4 a, Vec4 b) {
  a.x *= b.x;
  a.y *= b.y;
  a.z *= b.z;
  a.w *= b.w;
  return a;
}

[[nodiscard]] Vec4
operator/(Vec4 a, Vec4 b) {
  dbg_check_(!f32_compare(b.x, 0));
  dbg_check_(!f32_compare(b.y, 0));
  dbg_check_(!f32_compare(b.z, 0));
  dbg_check_(!f32_compare(b.w, 0));
  a.x /= b.x;
  a.y /= b.y;
  a.z /= b.z;
  a.w /= b.w;
  return a;
}

[[nodiscard]] Vec4
operator*(Vec4 v, f32 x) {
  v.x *= x;
  v.y *= x;
  v.z *= x;
  v.w *= x;
  return v;
}

Vec4 &
operator+=(Vec4 &a, Vec4 b) {
  a = a + b;
  return a;
}

Vec4 &
operator-=(Vec4 &a, Vec4 b) {
  a = a - b;
  return a;
}

Vec4 &
operator*=(Vec4 &a, Vec4 b) {
  a = a * b;
  return a;
}

Vec4 &
operator/=(Vec4 &a, Vec4 b) {
  a = a / b;
  return a;
}

Vec4 &
operator*=(Vec4 &a, f32 x) {
  a = a * x;
  return a;
}
} // namespace rt
