namespace rt {
[[nodiscard]] f32
random_f32() {
  // @crt
  return ::rand() / (RAND_MAX + 1.0f);
}

[[nodiscard]] f32
random_f32_in_range(f32 min, f32 max) {
  return min + (max - min) * random_f32();
}

[[nodiscard]] Vec3
random_vec3() {
  Vec3 const result = {.x = random_f32(), .y = random_f32(), .z = random_f32()};

  return result;
}

[[nodiscard]] Vec3
random_vec3_in_range(f32 min, f32 max) {
  Vec3 const result = random_vec3() * (max - min) + Vec3 {min, min, min};

  return result;
}

[[nodiscard]] Vec3
random_in_unit_sphere() {
  // @Robustness @Performance doesn't look too good
  while (true) {
    Vec3 const point = random_vec3();
    if (len_sq(point) >= 1) {
      continue;
    }

    return point;
  }
}

[[nodiscard]] Vec3
random_unit_vector() {
  return normalized(random_in_unit_sphere());
}

[[nodiscard]] Vec3
random_in_hemisphere(Vec3 normal) {
  Vec3 const in_unit_sphere = random_in_unit_sphere();
  f32 const  d              = dot(in_unit_sphere, normal);

  Vec3 result;
  if (d > 0.0) {
    result = in_unit_sphere;
  } else {
    result = in_unit_sphere * -1.f;
  }

  return result;
}

[[nodiscard]] Vec3
random_in_unit_disk() {
  // @Robustness @Performance doesn't look too good
  while (true) {
    Vec3 const p = {random_f32_in_range(-1, 1), random_f32_in_range(-1, 1), 0};

    if (len_sq(p) >= 1) {
      continue;
    }

    return p;
  }
}
} // namespace rt
