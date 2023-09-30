namespace rt {
struct World {
  Sphere *spheres;
  s32     num_spheres_reserved;
  s32     num_spheres;
  AABB    aabb;
};

enum World_Type { WorldType_Book1Final = 0, WorldType_Test };

[[nodiscard]] World
create_world(World_Type type);

void
add_sphere(World &w, Sphere s, Material *mat);
} // namespace rt
