namespace rt {
[[nodiscard]] Sphere
make_sphere(Vec3 center, f32 r) {
  Vec3 const radius_vec {.x = r, .y = r, .z = r};
  Vec3 const aabb_min = center - radius_vec;
  Vec3 const aabb_max = center + radius_vec;

  return {.center   = center,
          .radius   = r,
          .aabb     = make_aabb_from_extremes(aabb_min, aabb_max),
          .material = NULL};
}
} // namespace rt
