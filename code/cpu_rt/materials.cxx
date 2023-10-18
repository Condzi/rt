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

// Schlick's approximation of reflectance
[[nodiscard]] f32
reflectance(f32 cosine, f32 refraction_index) {
  f32 r0 = (1 - refraction_index) / (1 + refraction_index);
  r0     = r0 * r0;

  return r0 + (1 - r0) * ::powf((1 - cosine), 5);
}

/**
 * Specializations for different materials
 */

[[nodiscard]] bool
scatter_lambertian(Material const &material,
                   Hit_Info const &hi,
                   Vec3           &attenuation_color,
                   Ray            &out) {
#if 0 // Alternative formulas
    Vec3 scatter_direction = hi.normal + random_in_unit_sphere();
    Vec3 scatter_direction = random_in_hemisphere(hi.normal);
#endif
  Vec3 scatter_direction = hi.normal + random_unit_vector();
  if (near_zero(scatter_direction)) {
    scatter_direction = hi.normal;
  }

  out               = make_ray(hi.p, scatter_direction);
  attenuation_color = material.lambertian.albedo;

  return true;
}

[[nodiscard]] bool
scatter_metal(Material const &material,
              Ray const      &in,
              Hit_Info const &hi,
              Vec3           &attenuation_color,
              Ray            &out) {
  Vec3 const reflected = reflect(normalized(in.direction), hi.normal);
  Vec3 const direction = reflected + random_in_unit_sphere() * material.metal.fuzz;
  out                  = make_ray(hi.p, direction);

  attenuation_color = material.metal.albedo;
  return (dot(out.direction, hi.normal) > 0);
}

[[nodiscard]] bool
scatter_dielectric(Material const &material,
                   Ray const      &in,
                   Hit_Info const &hi,
                   Vec3           &attenuation_color,
                   Ray            &out) {
  f32 const refraction_ratio = hi.front_face ? (1.0f / material.dielectric.refraction_index)
                                             : (material.dielectric.refraction_index);

  Vec3 const unit_direction = normalized(in.direction);
  f32 const  cos_theta      = fmin(dot(unit_direction * -1.f, hi.normal), 1.0);
  f32 const  sin_theta      = ::sqrt(1.0f - cos_theta * cos_theta);

  bool const can_refract = (refraction_ratio * sin_theta) <= 1.0f;
  bool const reflectance_test =
      reflectance(cos_theta, refraction_ratio) > random_f32();

  Vec3 direction;
  if (can_refract && !reflectance_test) {
    direction = refract(unit_direction, hi.normal, refraction_ratio);
  } else {
    direction = reflect(unit_direction, hi.normal);
  }

  out               = make_ray(hi.p, direction);
  attenuation_color = Vec3 {1.0f, 1.0f, 1.0f};

  return true;
}

[[nodiscard]] Vec3
emit_diffuse_light(Material const &material) {
  return material.diffuse_light.albedo;
}

/**
 * Actual scatter and emit definitions
 */

[[nodiscard]] bool
scatter(Material const &material,
        Ray const      &in,
        Hit_Info const &hi,
        Vec3           &attenuation_color,
        Ray            &out) {

  switch (material.type) {
    case MaterialType_Lambertian: {
      return scatter_lambertian(material, hi, attenuation_color, out);
    } break;

    case MaterialType_Metal: {
      return scatter_metal(material, in, hi, attenuation_color, out);
    } break;

    case MaterialType_Dielectric: {
      return scatter_dielectric(material, in, hi, attenuation_color, out);
    } break;

    default: {
      return false;
    }
  }
}

[[nodiscard]] Vec3
emit(Material const &material) {
  if (material.type == MaterialType_Diffuse_Light) {
    return emit_diffuse_light(material);
  }

  return {0, 0, 0};
}

[[nodiscard]] Material
make_lambertian(Vec3 const &albedo) {
  return {.type = MaterialType_Lambertian, .lambertian {.albedo = albedo}};
}

[[nodiscard]] Material
make_metal(Vec3 const &albedo, f32 fuzz) {
  return {.type = MaterialType_Metal, .metal {.albedo = albedo, .fuzz = fuzz}};
}

[[nodiscard]] Material
make_dielectric(f32 refraction_index) {
  return {.type = MaterialType_Dielectric,
          .dielectric {.refraction_index = refraction_index}};
}

[[nodiscard]] Material
make_diffuse_light(Vec3 const &albedo) {
  return {.type = MaterialType_Diffuse_Light, .diffuse_light {.albedo = albedo}};
}
} // namespace rt
