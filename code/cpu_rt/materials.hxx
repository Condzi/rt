namespace rt {
struct Hit_Info;

enum Material_Type {
  MaterialType_None = 0,
  MaterialType_Lambertian,
  MaterialType_Metal,
  MaterialType_Dielectric,
  MaterialType_Diffuse_Light,

  MaterialType__count
};

struct Material {
  Material_Type type;
  union {
    struct {
      Vec3 albedo;
    } lambertian;
    struct {
      Vec3 albedo;
      f32  fuzz;
    } metal;
    struct {
      f32 refraction_index;
    } dielectric;
    struct {
      Vec3 albedo;
    } diffuse_light;
  };
};

[[nodiscard]] bool
scatter(Material const &material,
        Ray const      &in,
        Hit_Info const &hi,
        Vec3           &attenuation_color,
        Ray            &out);

[[nodiscard]] Vec3
emit(Material const &material);

[[nodiscard]] Material
make_lambertian(Vec3 const &albedo);

[[nodiscard]] Material
make_metal(Vec3 const &albedo, f32 fuzz);

[[nodiscard]] Material
make_dielectric(f32 refraction_index);

[[nodiscard]] Material
make_diffuse_light(Vec3 const &albedo);
} // namespace rt
