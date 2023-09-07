namespace rt {
// 3D AABB based on the slab method
// @Todo For extra speed with vectorized instr.,
//       see Ingo Wald
struct AABB {
  Vec2 x, y, z; // Intervals on axes
};

[[nodiscard]] AABB
make_aabb_from_intervals(Vec2 x, Vec2 y, Vec2 z);

[[nodiscard]] AABB
make_aabb_from_extremas(Vec3 a, Vec3 b);

[[nodiscard]] AABB
make_aabb_from_aabbs(AABB const &a, AABB const &b);

[[nodiscard]] bool
ray_vs_aabb(Vec3 ray_origin, Vec3 ray_direction, Vec2 ray_t, AABB const &aabb);

} // namespace rt
