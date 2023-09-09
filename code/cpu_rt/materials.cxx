namespace rt {
/**
 * Helper math functions
 */

[[nodiscard]] Vec3
reflect(Vec3 v, Vec3 n) {
  return v - n * 2 * dot(v, n);
}

// from Snell's law
[[nodiscard]] Vec3
refract(Vec3 uv, Vec3 n, f32 etai_over_etat) {
  f32 const  cos_theta      = fmin(dot(uv * -1.f, n), 1.0);
  Vec3 const r_out_perp     = (uv + n * cos_theta) * etai_over_etat;
  Vec3 const r_out_parallel = n * (-::sqrtf(::fabsf(1.0f - len_sq(r_out_perp))));

  return r_out_perp + r_out_parallel;
}

// Schlick's approximation of reflactance
[[nodiscard]] f32
reflactance(f32 cosine, f32 refraction_index) {
  f32 r0 = (1 - refraction_index) / (1 + refraction_index);
  r0     = r0 * r0;

  return r0 + (1 - r0) * ::powf((1 - cosine), 5);
}

Lambertian::Lambertian(Vec3 albedo_) : albedo(albedo_) {};

[[nodiscard]] bool
Lambertian::scatter(Ray const      &in,
                    Hit_Info const &hi,
                    Vec3           &attenuation_color,
                    Ray            &out) {
  (void)in;
#if 0 // Alternative formulas
    Vec3 scatter_direction = hi.normal + random_in_unit_sphere();
    Vec3 scatter_direction = random_in_hemisphere(hi.normal);
#endif
  Vec3 scatter_direction = hi.normal + random_unit_vector();
  if (near_zero(scatter_direction)) {
    scatter_direction = hi.normal;
  }

  out               = {.origin        = hi.p,
                       .direction     = scatter_direction,
                       .direction_inv = Vec3 {1, 1, 1} / scatter_direction};
  attenuation_color = albedo;

  return true;
}

Metal::Metal(Vec3 albedo_, f32 fuzz_)
    : albedo(albedo_), fuzz(fuzz_ < 1 ? fuzz_ : 1) {}

[[nodiscard]] bool
Metal::scatter(Ray const &in, Hit_Info const &hi, Vec3 &attenuation_color, Ray &out) {
  Vec3 const reflected = reflect(normalized(in.direction), hi.normal);
  Vec3 const direction = reflected + random_in_unit_sphere() * fuzz;
  out                  = {.origin        = hi.p,
                          .direction     = direction,
                          .direction_inv = Vec3 {1, 1, 1} / direction};

  attenuation_color = albedo;
  return (dot(out.direction, hi.normal) > 0);
}

Dielectric::Dielectric(f32 refraction_index_) : refraction_index(refraction_index_) {}

[[nodiscard]] bool
Dielectric::scatter(Ray const      &in,
                    Hit_Info const &hi,
                    Vec3           &attenuation_color,
                    Ray            &out) {
  f32 const refraction_ratio =
      hi.front_face ? (1.0f / refraction_index) : (refraction_index);

  Vec3 const unit_direction = normalized(in.direction);
  f32 const  cos_theta      = fmin(dot(unit_direction * -1.f, hi.normal), 1.0);
  f32 const  sin_theta      = ::sqrt(1.0f - cos_theta * cos_theta);

  bool const can_refract = (refraction_ratio * sin_theta) <= 1.0f;
  bool const reflactance_test =
      reflactance(cos_theta, refraction_ratio) > random_f32();

  Vec3 direction;
  if (can_refract && !reflactance_test) {
    direction = refract(unit_direction, hi.normal, refraction_ratio);
  } else {
    direction = reflect(unit_direction, hi.normal);
  }

  out               = {.origin = hi.p, .direction = direction, .direction_inv = Vec3{1,1,1}/direction};
  attenuation_color = Vec3 {1.0f, 1.0f, 1.0f};

  return true;
}
} // namespace rt
