namespace rt {
struct World {
  Sphere *spheres;
  s32     num_spheres_reserved;
  s32     num_spheres;
  Quad   *quads;
  s32     num_quads_reserved;
  s32     num_quads;
  AABB    aabb;
};

enum World_Type { WorldType_Book1Final = 0, WorldType_Quads, WorldType_SimpleLights };

[[nodiscard]] World
create_world(World_Type type);

void
add_sphere(World &w, Sphere s, Material *mat);

void
add_quad(World &w, Quad q, Material *mat);
} // namespace rt
