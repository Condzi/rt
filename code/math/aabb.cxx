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

[[nodiscard]] f32
surface_area(AABB const &a) {
  f32 const x = a.x.max - a.x.min;
  f32 const y = a.y.max - a.y.min;
  f32 const z = a.z.max - a.z.min;

  return 2.0f * (x * y + x * z + y * z);
}

[[nodiscard]] bool
ray_vs_aabb(Vec3 const &RT_RESTRICT ray_origin,
            Vec3 const &RT_RESTRICT ray_direction_inv,
            Vec2                    ray_t,
            AABB const &RT_RESTRICT aabb) {
  // @Note: Optimized method by Andrew Kensler
  for (int a = 0; a < 3; a++) {
    f32 const invD = ray_direction_inv.v[a];
    f32 const orig = ray_origin.v[a];

    f32 t0 = (aabb.v[a].min - orig) * invD;
    f32 t1 = (aabb.v[a].max - orig) * invD;

    if (invD < 0) std::swap(t0, t1);

    if (t0 > ray_t.min) ray_t.min = t0;
    if (t1 < ray_t.max) ray_t.max = t1;

    if (ray_t.max <= ray_t.min) return false;
  }
  return true;
}
} // namespace rt
