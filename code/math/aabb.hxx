namespace rt {
// 3D AABB based on the slab method
// @Todo For extra speed with vectorized instr.,
//       see Ingo Wald
union AABB {
  struct {
    Vec2 x, y, z; // Intervals on axes
  };

  Vec2 v[3] = {};
};

[[nodiscard]] AABB
make_aabb_from_intervals(Vec2 x, Vec2 y, Vec2 z);

[[nodiscard]] AABB
make_aabb_from_extremes(Vec3 a, Vec3 b);

[[nodiscard]] AABB
make_aabb_from_aabbs(AABB const &a, AABB const &b);

[[nodiscard]] f32
surface_area(AABB const &a);

[[nodiscard]] bool
ray_vs_aabb(Vec3 const &RT_RESTRICT ray_origin,
            Vec3 const &RT_RESTRICT ray_direction_inv,
            Vec2                    ray_t,
            AABB const &RT_RESTRICT aabb);

// If a side is too narrow it will mess up with BVH and ray vs aabb calculations,
// so we expand it a bit.
[[nodiscard]] AABB
add_padding_if_too_narrow(AABB aabb);
} // namespace rt
