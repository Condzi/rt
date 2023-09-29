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
    f32 const t0 = (aabb.v[a].min - ray_origin.v[a]) * ray_direction_inv.v[a];
    f32 const t1 = (aabb.v[a].max - ray_origin.v[a]) * ray_direction_inv.v[a];

    ray_t.min = std::max(ray_t.min, std::min(t0, t1));
    ray_t.max = std::min(ray_t.max, std::max(t0, t1));
  }

  return ray_t.max >= std::max(0.0f, ray_t.min);
}
} // namespace rt
