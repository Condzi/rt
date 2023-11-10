namespace rt {
[[nodiscard]] Camera
make_camera(Camera_Parameters const &params) {
  f32 const theta = 3.1415f * params.vfov / 180;
  f32 const h     = ::tanf(theta / 2);

  f32 const viewport_height = 2.0f * h;
  f32 const viewport_width  = params.aspect_ratio * viewport_height;

  Vec3 const w = normalized(params.center - params.look_at);
  Vec3 const u = normalized(cross(params.up, w));
  Vec3 const v = cross(w, u);

  Vec3 const origin     = params.center;
  Vec3 const horizontal = u * viewport_width * params.focus_distance;
  Vec3 const vertical   = v * viewport_height * params.focus_distance;
  Vec3 const lower_left_corner =
      origin - horizontal * 0.5 - vertical * 0.5 - w * params.focus_distance;

  Camera result = {.origin            = origin,
                   .horizontal        = horizontal,
                   .vertical          = vertical,
                   .lower_left_corner = lower_left_corner,
                   .u                 = u,
                   .v                 = v,
                   .w                 = w,
                   .lens_radius       = params.aperture / 2};

  return result;
} // namespace rt

[[nodiscard]] Ray
get_ray_at(Camera const &cam, f32 s, f32 t) {
  Vec3 const rd     = random_in_unit_disk() * cam.lens_radius;
  Vec3 const offset = cam.u * rd.x + cam.v * rd.y;

  Vec3 const origin    = cam.origin + offset;
  Vec3 const direction = cam.lower_left_corner + cam.horizontal * s +
                         cam.vertical * t - cam.origin - offset;

  return make_ray(origin, direction);
}
} // namespace rt
