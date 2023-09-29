namespace rt {
struct Material;

struct Sphere {
  Vec3 center;
  // @Note: can be negative: surface normals will point inward.
  f32  radius;
  AABB aabb;
  // Move this somewhere else?
  Material *material;
};

// Creates a sphere with empty material.
[[nodiscard]] Sphere
make_sphere(Vec3 center, f32 r);
} // namespace rt
