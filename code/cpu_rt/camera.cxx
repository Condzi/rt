namespace rt {
[[nodiscard]] Camera
make_camera(Vec3 center,
            Vec3 look_at,
            Vec3 up,
            f32  vfow,
            f32  aspect_ratio,
            f32  aperture,
            f32  focus_distance) {
  f32 const theta = 3.1415f * vfow / 180;
  f32 const h     = ::tanf(theta / 2);

  f32 const viewport_height = 2.0f * h;
  f32 const viewport_width  = aspect_ratio * viewport_height;

  Vec3 const w = normalized(center - look_at);
  Vec3 const u = normalized(cross(up, w));
  Vec3 const v = cross(w, u);

  Vec3 const origin     = center;
  Vec3 const horizontal = u * viewport_width * focus_distance;
  Vec3 const vertical   = v * viewport_height * focus_distance;
  Vec3 const lower_left_corner =
      origin - horizontal * 0.5 - vertical * 0.5 - w * focus_distance;

  Camera result = {.origin            = origin,
                   .horizontal        = horizontal,
                   .vertical          = vertical,
                   .lower_left_corner = lower_left_corner,
                   .u                 = u,
                   .v                 = v,
                   .w                 = w,
                   .lens_radius       = aperture / 2};

  return result;
} // namespace rt

[[nodiscard]] Ray
get_ray_at(Camera const &cam, f32 s, f32 t) {
  Vec3 const rd     = random_in_unit_disk() * cam.lens_radius;
  Vec3 const offset = cam.u * rd.x + cam.v * rd.y;

  Ray result = {.origin    = cam.origin + offset,
                .direction = cam.lower_left_corner + cam.horizontal * s +
                             cam.vertical * t - cam.origin - offset};
  return result;
}
} // namespace rt
