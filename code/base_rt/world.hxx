namespace rt {
struct World {
  Vec3 background_color;

  Sphere *spheres;
  s32     num_spheres_reserved;
  s32     num_spheres;
  Quad   *quads;
  s32     num_quads_reserved;
  s32     num_quads;
  AABB    aabb;

  std::vector<Material> materials;
};

enum World_Type {
  WorldType_Book1Final = 0,
  WorldType_Quads,
  WorldType_SimpleLights,
  WorldType__count
};

[[nodiscard]] World
create_world(World_Type type);

[[nodiscard]] std::vector<BVH_Flat>
world_create_bvh(World const &w);

void
add_sphere(World &w, Sphere s, Material *mat);

void
add_quad(World &w, Quad q, Material *mat);
} // namespace rt
