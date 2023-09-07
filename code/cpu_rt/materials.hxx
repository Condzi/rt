namespace rt {
struct Hit_Info;

// @TODO: remove polimorphism; use function pointers instead so we can use malloc
//        and don't have to care about vtable ptr. -- kkubacki 07.09.2023
struct Material {
  // Returns true when the ray was absorbed.
  // Calculates the new color and ray.
  virtual bool
  scatter(Ray const &in, Hit_Info const &hi, Vec3 &attenuation_color, Ray &out) = 0;
};

struct Lambertian : Material {
  Vec3 albedo;

  Lambertian(Vec3 albedo_);

  [[nodiscard]] bool
  scatter(Ray const      &in,
          Hit_Info const &hi,
          Vec3           &attenuation_color,
          Ray            &out) override;
};

struct Metal : Material {
  Vec3 albedo;
  f32  fuzz;

  Metal(Vec3 albedo_, f32 fuzz_);

  [[nodiscard]] bool
  scatter(Ray const      &in,
          Hit_Info const &hi,
          Vec3           &attenuation_color,
          Ray            &out) override;
};

struct Dielectric : Material {
  f32 refraction_index;

  Dielectric(f32 refraction_index_);

  [[nodiscard]] bool
  scatter(Ray const      &in,
          Hit_Info const &hi,
          Vec3           &attenuation_color,
          Ray            &out) override;
};
} // namespace rt
