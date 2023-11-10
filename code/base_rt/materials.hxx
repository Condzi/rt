namespace rt {
enum Material_Type : uint16_t {
  MaterialType_None = 0,
  MaterialType_Lambertian,
  MaterialType_Metal,
  MaterialType_Dielectric,
  MaterialType_Diffuse_Light,

  MaterialType__count
};

// @Note: redo this. 0 should not be a valid material id.
using Material_ID = uint32_t;

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

[[nodiscard]] Material
make_lambertian(Vec3 const &albedo);

[[nodiscard]] Material
make_metal(Vec3 const &albedo, f32 fuzz);

[[nodiscard]] Material
make_dielectric(f32 refraction_index);

[[nodiscard]] Material
make_diffuse_light(Vec3 const &albedo);
} // namespace rt