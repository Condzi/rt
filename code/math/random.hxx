namespace rt {
[[nodiscard]] f32
random_f32();

[[nodiscard]] f32
random_f32_in_range(f32 min, f32 max);

[[nodiscard]] Vec3
random_vec3();

[[nodiscard]] Vec3
random_vec3_in_range(f32 min, f32 max);

[[nodiscard]] Vec3
random_in_unit_sphere();

[[nodiscard]] Vec3
random_unit_vector();

[[nodiscard]] Vec3
random_in_hemisphere(Vec3 normal);
} // namespace rt
