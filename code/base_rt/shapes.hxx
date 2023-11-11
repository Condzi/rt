namespace rt {
struct Sphere {
  Vec3 center;
  // @Note: can be negative: surface normals will point inward.
  f32  radius;

  AABB aabb;
  Material_ID mat_id;
};

// Creates a sphere with empty material.
[[nodiscard]] Sphere
make_sphere(Vec3 center, f32 r);

struct Quad {
  Vec3 Q;    //< lower-left corner
  Vec3 u, v; // Sides, relative to the corner.

  // Used for ray-quad calculations
  Vec3 normal;
  f32  D;
  Vec3 w;

  AABB      aabb;
  Material_ID mat_id;
};

// Create a quad with empty material.
[[nodiscard]] Quad
make_quad(Vec3 Q, Vec3 u, Vec3 v);
} // namespace rt
