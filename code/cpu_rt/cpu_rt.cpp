namespace rt {
struct Ray {
  Vec3 origin;
  Vec3 direction;
};

[[nodiscard]] Vec3
at(Ray const &r, double t) {
  return r.origin + r.direction * (f32)t;
}

[[nodiscard]] Vec3
ray_color(Ray const &r) {
  Vec3   dir    = normalized(r.direction);
  double t      = 0.5 * (dir.y + 1.0);
  double scalar = 1 - t;

  Vec3 color = Vec3 {1, 1, 1} * (f32)scalar + Vec3 {0.5, 0.7, 1.0} * (f32)t;
  return color;
}

[[nodiscard]] Rt_Output
do_raytraycing() {
  double const aspect_ratio = 16.0 / 9;
  s32 const    image_width  = 400;
  s32 const    image_height = (s32)(image_width / aspect_ratio);

  // Camera

  double const viewport_height = 2.0;
  double const viewport_width  = aspect_ratio * viewport_height;
  double const focal_length    = 1.0;

  Vec3 const origin     = Vec3 {0, 0, 0};
  Vec3 const horizontal = Vec3 {viewport_width, 0, 0};
  Vec3 const vertical   = Vec3 {0, viewport_height, 0};
  Vec3 const lower_left_corner =
      origin - horizontal * 0.5 - vertical * 0.5 - Vec3 {0, 0, focal_length};

  // Render

  u8 *buffer = (u8*)alloc_perm(image_width * image_height * 3);

  for (int j = image_height - 1; j >= 0; --j) {
    logf("Scanlines remaining: %d\n", j);
    for (int i = 0; i < image_width; ++i) {
      auto u = double(i) / (image_width - 1);
      auto v = double(j) / (image_height - 1);
      Ray  r {origin, lower_left_corner + horizontal*(f32)u + vertical*(f32)v - origin};
      Vec3 pixel_color = ray_color(r);

      buffer[3 * (j * image_width + i)]     = u8(pixel_color.r * 255.999);
      buffer[3 * (j * image_width + i) + 1] = u8(pixel_color.g * 255.999);
      buffer[3 * (j * image_width + i) + 2] = u8(pixel_color.b * 255.999);
    }
  }

  return {.image_size = {(f32)image_width, (f32)image_height},
          .rgb_data   = {.count = image_width * image_height * 3, .bytes = buffer}};
}
} // namespace rt
