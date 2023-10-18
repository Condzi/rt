namespace rt {
struct Hit_Info;

// @TODO: remove polymorphism; use function pointers instead so we can use malloc
//        and don't have to care about vtable ptr. -- kkubacki 07.09.2023
struct Material {
  // Returns true when the ray was absorbed.
  // Calculates the new color and ray.
  virtual bool
  scatter(Ray const &in, Hit_Info const &hi, Vec3 &attenuation_color, Ray &out) = 0;

  virtual Vec3
  emitted() {
    return {0, 0, 0};
  }
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

struct Diffuse_Light : Material {
  Vec3 color;
  Diffuse_Light(Vec3 col_) : color(col_) {};

  bool
  scatter(Ray const &, Hit_Info const &, Vec3 &, Ray &) {
    return false;
  }

  virtual Vec3
  emitted() {
    return color;
  }
};
} // namespace rt
