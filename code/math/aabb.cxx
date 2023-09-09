namespace rt {
[[nodiscard]] AABB
make_aabb_from_intervals(Vec2 x, Vec2 y, Vec2 z) {
  return {.x = x, .y = y, .z = z};
}

[[nodiscard]] AABB
make_aabb_from_extremas(Vec3 a, Vec3 b) {
  return {
      .x = {.min = ::fmin(a.x, b.x), .max = ::fmax(a.x, b.x)},
      .y = {.min = ::fmin(a.y, b.y), .max = ::fmax(a.y, b.y)},
      .z = {.min = ::fmin(a.z, b.z), .max = ::fmax(a.z, b.z)},
  };
}

[[nodiscard]] AABB
make_aabb_from_aabbs(AABB const &a, AABB const &b) {
  return {.x = {.min = ::fmin(a.x.min, b.x.min), .max = ::fmax(a.x.max, b.x.max)},
          .y = {.min = ::fmin(a.y.min, b.y.min), .max = ::fmax(a.y.max, b.y.max)},
          .z = {.min = ::fmin(a.z.min, b.z.min), .max = ::fmax(a.z.max, b.z.max)}};
}

[[nodiscard]] bool
ray_vs_aabb(Vec3 ray_origin, Vec3 ray_direction_inv, Vec2 ray_t, AABB const &aabb) {
  // @Note: originally in the book this is in a loop, but to avoid adding mental
  //        complexity & improve performance, I unrolled it.
  // @Note: Optimized method by Andrew Kensler

  /**
   * X axis
   */
  f32 invD = ray_direction_inv.x;
  f32 orig = ray_origin.x;

  f32 t0 = (aabb.x.min - orig) * invD;
  f32 t1 = (aabb.x.max - orig) * invD;

  if (invD < 0) rt_swap(t0, t1);

  if (t0 > ray_t.min) ray_t.min = t0;
  if (t1 < ray_t.max) ray_t.max = t1;

  if (ray_t.max <= ray_t.min) return false;

  /**
   * Y axis
   */
  invD = ray_direction_inv.y;
  orig = ray_origin.y;

  t0 = (aabb.y.min - orig) * invD;
  t1 = (aabb.y.max - orig) * invD;

  if (invD < 0) rt_swap(t0, t1);

  if (t0 > ray_t.min) ray_t.min = t0;
  if (t1 < ray_t.max) ray_t.max = t1;

  if (ray_t.max <= ray_t.min) return false;

  /**
   * Z axis
   */
  invD = ray_direction_inv.z;
  orig = ray_origin.z;

  t0 = (aabb.z.min - orig) * invD;
  t1 = (aabb.z.max - orig) * invD;

  if (invD < 0) rt_swap(t0, t1);

  if (t0 > ray_t.min) ray_t.min = t0;
  if (t1 < ray_t.max) ray_t.max = t1;

  if (ray_t.max <= ray_t.min) return false;

  // All axis overleap, we got a hit
  return true;
}
} // namespace rt
