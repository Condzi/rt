namespace rt {
[[nodiscard]] Camera
make_camera() {
  f32 const aspect_ratio    = 16.0 / 9;
  f32 const viewport_height = 2.0;
  f32 const viewport_width  = aspect_ratio * viewport_height;
  f32 const focal_length    = 1.0;

  Vec3 const origin     = {0, 0, 0};
  Vec3 const horizontal = {viewport_width, 0, 0};
  Vec3 const vertical   = {0, viewport_height, 0};
  Vec3 const lower_left_corner =
      origin - horizontal * 0.5 - vertical * 0.5 - Vec3 {0, 0, focal_length};

  Camera result = {.origin            = origin,
                   .horizontal        = horizontal,
                   .vertical          = vertical,
                   .lower_left_corner = lower_left_corner};

  return result;
} // namespace rt

[[nodiscard]] Ray
get_ray_at(Camera const &cam, f32 u, f32 v) {
  Ray result = {.origin    = cam.origin,
                .direction = cam.lower_left_corner + cam.horizontal * u +
                             cam.vertical * v - cam.origin};
  return result;
}
} // namespace rt
