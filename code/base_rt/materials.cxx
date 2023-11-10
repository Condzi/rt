namespace rt {
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
