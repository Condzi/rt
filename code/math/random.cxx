namespace rt {
[[nodiscard]] f32
random_f32() {
  // @crt
  return ::rand() / (RAND_MAX + 1.0f);
}

[[nodiscard]] f32
random_f32_in_range(f32 min, f32 max) {
  return min + (max - min)*random_f32();
}
} // namespace rt
