namespace rt {
[[nodiscard]] Camera
make_camera(Vec3 center, Vec3 look_at, Vec3 up, f32 vfow, f32 aspect_ratio) {
  f32 const theta = 3.1415f * vfow / 180;
  f32 const h     = ::tanf(theta / 2);

  f32 const viewport_height = 2.0f * h;
  f32 const viewport_width  = aspect_ratio * viewport_height;

  Vec3 const w = normalized(center - look_at);
  Vec3 const u = normalized(cross(up, w));
  Vec3 const v = cross(w, u);

  Vec3 const origin     = center;
  Vec3 const horizontal = u*viewport_width;
  Vec3 const vertical   = v*viewport_height;
  Vec3 const lower_left_corner =
      origin - horizontal * 0.5 - vertical * 0.5 - w;

  Camera result = {.origin            = origin,
                   .horizontal        = horizontal,
                   .vertical          = vertical,
                   .lower_left_corner = lower_left_corner};

  return result;
} // namespace rt

[[nodiscard]] Ray
get_ray_at(Camera const &cam, f32 s, f32 t) {
  Ray result = {.origin    = cam.origin,
                .direction = cam.lower_left_corner + cam.horizontal * s +
                             cam.vertical * t - cam.origin};
  return result;
}
} // namespace rt
