namespace rt {
// Scene sampling stuff
struct Camera {
  Vec3 origin;
  Vec3 horizontal;
  Vec3 vertical;
  Vec3 lower_left_corner;
  Vec3 u,v,w;
  f32 lens_radius;
};

struct Camera_Parameters {
  Vec3 center;
  Vec3 look_at;
  Vec3 up;
  f32  vfov;
  f32  aspect_ratio;
  f32  aperture;
  f32  focus_distance;
};

[[nodiscard]] Camera
make_camera(Camera_Parameters const& params);

[[nodiscard]] Ray
get_ray_at(Camera const &cam, f32 u, f32 v);
} // namespace rt
