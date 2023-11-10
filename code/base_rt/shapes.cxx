namespace rt {
[[nodiscard]] Sphere
make_sphere(Vec3 center, f32 r) {
  Vec3 const radius_vec {.x = r, .y = r, .z = r};
  Vec3 const aabb_min = center - radius_vec;
  Vec3 const aabb_max = center + radius_vec;

  return {.center = center,
          .radius = r,
          .aabb   = make_aabb_from_extremes(aabb_min, aabb_max),
          .mat_id = 0};
}

[[nodiscard]] Quad
make_quad(Vec3 Q, Vec3 u, Vec3 v) {
  Vec3 const aabb_min = Q;
  Vec3 const aabb_max = Q + u + v;

  AABB aabb = make_aabb_from_extremes(aabb_min, aabb_max);
  aabb      = add_padding_if_too_narrow(aabb);

  Vec3 n      = cross(u, v);
  Vec3 normal = normalized(n);
  f32  D      = dot(normal, Q);
  Vec3 w      = n * (1 / dot(n, n));

  return {.Q        = Q,
          .u        = u,
          .v        = v,
          .normal   = normal,
          .D        = D,
          .w        = w,
          .aabb     = aabb,
          .mat_id = 0};
}
} // namespace rt
