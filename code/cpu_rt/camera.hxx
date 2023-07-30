namespace rt {
// Scene sampling stuff
struct Camera {
  Vec3 origin;
  Vec3 horizontal;
  Vec3 vertical;
  Vec3 lower_left_corner;
};

[[nodiscard]] Camera
make_camera(Vec3 center, Vec3 look_at, Vec3 up, f32 vfow, f32 aspect_ratio);

[[nodiscard]] Ray
get_ray_at(Camera const &cam, f32 u, f32 v);
} // namespace rt
