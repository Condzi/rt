namespace rt {

[[nodiscard]] World
world_book1();

[[nodiscard]] World
world_quads();

[[nodiscard]] World
world_simple_lights();

[[nodiscard]] World
create_world(World_Type type) {
  switch (type) {
    case WorldType_Book1Final:
      return world_book1();
    case WorldType_Quads:
      return world_quads();
    case WorldType_SimpleLights:
      return world_simple_lights();
    default:
      assert(false);
  }

  return {};
}

void
add_sphere(World &w, Sphere s, Material const &mat) {
  assert(w.num_spheres < w.num_spheres_reserved);
  assert(mat.type);

  s32 const idx           = w.num_spheres++;
  w.spheres[idx]          = s;
  w.spheres[idx].mat_id   = (uint32_t)w.materials.size();
  w.aabb                  = make_aabb_from_aabbs(w.aabb, s.aabb);

  w.materials.emplace_back(mat);
}

void
add_quad(World &w, Quad q, Material const &mat) {
  assert(w.num_quads < w.num_quads_reserved);
  assert(mat.type);

  s32 const idx         = w.num_quads++;
  w.quads[idx]          = q;
  w.quads[idx].mat_id   = (uint32_t)w.materials.size();
  w.aabb                = make_aabb_from_aabbs(w.aabb, q.aabb);

  w.materials.emplace_back(mat);
}

[[nodiscard]] World
world_book1() {
  World w;
  w.background_color = {0.5, 0.7, 1.0};
  w.num_quads = w.num_quads_reserved = 0;
  w.quads                            = 0;
  w.num_spheres          = 0;
  w.aabb                 = AABB {};
  w.num_spheres_reserved = 22 * 22 + 2;
  w.spheres                          = perm<Sphere>(w.num_spheres_reserved);

  Material const ground_material = make_lambertian({0.5, 0.5, 0.5});
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
      Material  mat        = {0};
      if (choose_mat < 0.8f) {
        Vec3 const albedo = random_vec3() * random_vec3();
        mat               = make_lambertian(albedo);
      } else if (choose_mat < 0.95f) {
        Vec3 const albedo = random_vec3_in_range(0.5f, 1);
        f32 const  fuzz   = random_f32_in_range(0, 0.5f);
        mat               = make_metal(albedo, fuzz);
      } else {
        mat = make_dielectric(1.5f);
      }

      add_sphere(w, make_sphere(center, 0.2f), mat);
    }
  }
  logf("%d iterations \n", iteration_counter);

  Material const mat1 = make_dielectric(1.5f);
  Material const mat2 = make_lambertian({0.4f, 0.2f, 0.1f});
  Material const mat3 = make_metal(Vec3 {0.7f, 0.6f, 0.5f}, 0.0f);

  add_sphere(w, make_sphere(Vec3 {0, 1, 0}, 1.0), mat1);
  add_sphere(w, make_sphere(Vec3 {-4, 1, 0}, 1.0), mat2);
  add_sphere(w, make_sphere(Vec3 {4, 1, 0}, 1.0), mat3);

  logf("%d/%d spheres generated.\n", w.num_spheres, w.num_spheres_reserved);

  return w;
}

[[nodiscard]] World
world_quads() {
  World w              = {0};
  w.background_color = {0.5, 0.7, 1.0};
  w.num_quads_reserved = 5;
  w.quads              = perm<Quad>(w.num_quads_reserved);

  Material const red    = make_lambertian({1.0f, 0.2f, 0.2f});
  Material const green  = make_lambertian({0.2f, 1.0f, 0.2f});
  Material const blue   = make_lambertian({0.2f, 0.2f, 1.0f});
  Material const orange = make_lambertian({1.0f, 0.5f, 0.0f});
  Material const teal   = make_lambertian({0.2f, 0.8f, 0.8f});

  add_quad(w, make_quad({-3, -2, 5}, {0, 0, -4}, {0, 4, 0}), red);
  add_quad(w, make_quad({-2, -2, 0}, {4, 0, 0}, {0, 4, 0}), green);
  add_quad(w, make_quad({3, -2, 1}, {0, 0, 4}, {0, 4, 0}), blue);
  add_quad(w, make_quad({-2, 3, 1}, {4, 0, 0}, {0, 0, 4}), orange);
  add_quad(w, make_quad({-2, -3, 5}, {4, 0, 0}, {0, 0, -4}), teal);

  return w;
}

[[nodiscard]] World
world_simple_lights() {
  World w                = {0};
  w.background_color = Vec3{0.5, 0.7, 1.0} * 0.01f; // Default bg, dimmed
  w.num_spheres_reserved = 3;
  w.num_quads_reserved   = 1;
  w.quads                = perm<Quad>(w.num_quads_reserved);
  w.spheres              = perm<Sphere>(w.num_spheres_reserved);

  Material const ground_material = make_lambertian({1.0, 0.5, 0.5});
  Material const green           = make_lambertian({0.2f, 1.0f, 0.2f});
  Material const light           = make_diffuse_light({10, 10, 10});

  add_sphere(w, make_sphere(Vec3 {0, -1000, 0}, 1000), ground_material);
  add_sphere(w, make_sphere(Vec3 {0, 2, 0}, 2), green);
  add_sphere(w, make_sphere({0, 7, 0}, 2), light);
  add_quad(w, make_quad({3, 1, -2}, {2, 0, 0}, {0, 2, 0}), light);

  return w;
}
} // namespace rt