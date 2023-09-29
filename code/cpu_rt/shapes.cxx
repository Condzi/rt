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

[[nodiscard]] Quad
make_quad(Vec3 Q, Vec3 u, Vec3 v) {
  Vec3 const aabb_min = Q;
  Vec3 const aabb_max = Q + u + v;

  AABB aabb = make_aabb_from_extremes(aabb_min, aabb_max);
  aabb      = add_padding_if_too_narrow(aabb);

  return {.Q = Q, .u = u, .v = v, .aabb = aabb, .material = NULL};
}
} // namespace rt
