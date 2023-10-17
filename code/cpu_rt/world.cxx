namespace rt {

[[nodiscard]] World
world_book1();

[[nodiscard]] World
create_world(World_Type type) {
  switch (type) {
    case WorldType_Book1Final:
      return world_book1();
    case WorldType_Test:
      return World {}; // @ToDo
    default:
      assert(false);
  }

  return {};
}

void
add_sphere(World &w, Sphere s, Material *mat) {
  assert(w.num_spheres < w.num_spheres_reserved);

  s32 const idx           = w.num_spheres++;
  w.spheres[idx]          = s;
  w.spheres[idx].material = mat;
  w.aabb                  = make_aabb_from_aabbs(w.aabb, s.aabb);
}

[[nodiscard]] World
world_book1() {
  World w;
  w.num_quads = w.num_quads_reserved = 0;
  w.quads                            = 0;
  w.num_spheres          = 0;
  w.aabb                 = AABB {};
  w.num_spheres_reserved = 22 * 22 + 2;
  w.spheres = (Sphere *)alloc_perm(w.num_spheres_reserved * sizeof(Sphere));

  Lambertian *ground_material = (Lambertian *)alloc_perm(sizeof(Lambertian));
  new (ground_material) Lambertian {Vec3 {0.5, 0.5, 0.5}};
  add_sphere(w, make_sphere(Vec3 {0, -1000, 0}, 1000), ground_material);

  s32 iteration_counter = 0;

  for (s32 a = -11; a < 11; a++) {
    for (s32 b = -11; b < 11; b++) {
      iteration_counter++;
      // Space for 3 giant spheres, added outside the loop
      if (w.num_spheres_reserved - w.num_spheres == 3) {
        continue;
      }

      Vec3 const center {a + 0.9f * random_f32(), 0.2f, b + 0.9f * random_f32()};

      if (dist(center, Vec3 {4, 0.2f, 0}) < 0.9f) {
        continue;
      }

      f32 const choose_mat = random_f32();
      Material *mat;
      if (choose_mat < 0.8f) {
        Vec3 const albedo = random_vec3() * random_vec3();
        mat               = (Lambertian *)alloc_perm(sizeof(Lambertian));
        new (mat) Lambertian {albedo};
      } else if (choose_mat < 0.95f) {
        Vec3 const albedo = random_vec3_in_range(0.5f, 1);
        f32 const  fuzz   = random_f32_in_range(0, 0.5f);
        mat               = (Metal *)alloc_perm(sizeof(Metal));
        new (mat) Metal {albedo, fuzz};
      } else {
        mat = (Dielectric *)alloc_perm(sizeof(Dielectric));
        new (mat) Dielectric {1.5f};
      }

      add_sphere(w, make_sphere(center, 0.2f), mat);
    }
  }
  logf("%d iterations \n", iteration_counter);

  Dielectric *mat1 = (Dielectric *)alloc_perm(sizeof(Dielectric));
  new (mat1) Dielectric {1.5f};

  Lambertian *mat2 = (Lambertian *)alloc_perm(sizeof(Lambertian));
  new (mat2) Lambertian {Vec3 {0.4f, 0.2f, 0.1f}};

  Metal *mat3 = (Metal *)alloc_perm(sizeof(Metal));
  new (mat3) Metal {Vec3 {0.7f, 0.6f, 0.5f}, 0.0f};

  add_sphere(w, make_sphere(Vec3 {0, 1, 0}, 1.0), mat1);
  add_sphere(w, make_sphere(Vec3 {-4, 1, 0}, 1.0), mat2);
  add_sphere(w, make_sphere(Vec3 {4, 1, 0}, 1.0), mat3);

  logf("%d/%d spheres generated.\n", w.num_spheres, w.num_spheres_reserved);

  return w;
}
} // namespace rt
