namespace rt {
struct Mat4x4 {
  f32 v[4][4];
};

[[nodiscard]] Mat4x4
identity();

[[nodiscard]] Vec4
transformed_point(Vec4 const &p, Mat4x4 const &m);
[[nodiscard]] Vec2
transformed_point(Vec2 const &p, Mat4x4 const &m);

Vec4 &
transform_point(Vec4 &p, Mat4x4 const &m);
Vec2 &
transform_point(Vec2 &p, Mat4x4 const &m);

[[nodiscard]] Mat4x4
combine(Mat4x4 const &a, Mat4x4 const &b);

[[nodiscard]] Mat4x4
transpose(Mat4x4 m);

[[nodiscard]] Mat4x4
rot_z(f32 angle);
[[nodiscard]] Mat4x4
scale2(Vec2 factor);

[[nodiscard]] Mat4x4
translate2(Vec2 o);

[[nodiscard]] Mat4x4
ortho_proj(f32 width, f32 height);
} // namespace rt