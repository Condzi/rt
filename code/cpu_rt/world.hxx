namespace rt {
struct World {
  Sphere *spheres;
  s32     num_spheres_reserved;
  s32     num_spheres;
  AABB    aabb;
};

void
add_sphere(World &w, Sphere s, Material *mat);

[[nodiscard]] World
random_scene();
} // namespace rt
