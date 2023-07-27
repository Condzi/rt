namespace rt {
// Scene sampling stuff
struct Camera {
  Vec3 origin;
  Vec3 horizontal;
  Vec3 vertical;
  Vec3 lower_left_corner;
};

[[nodiscard]] Camera
make_camera();

[[nodiscard]] Ray
get_ray_at(Camera const &cam, f32 u, f32 v);
} // namespace rt
