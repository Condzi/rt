/**
 * Vector types expressed as unions for better expression.
*/

namespace rt {
union Vec2 {
  struct {
    f32 x, y;
  };

  struct {
    f32 width, height;
  };

  f32 v[2] = {};
};

union Vec3 {
  struct {
    f32 x, y, z;
  };

  struct {
    Vec2 xy; f32 _unused0;
  };

  f32 v[3] = {};
};

union Vec4 {
  struct {
    f32 x, y, z, w;
  };

  struct {
    f32 r, g, b, a;
  };

  struct {
    Vec3 xyz; f32 _unused0;
  };

  struct {
    Vec2 xy; Vec2 _unused2;
  };

  f32 v[4] = {};
};

/**
 * Functions
*/

[[nodiscard]] f32 dist_sq(Vec2 a, Vec2 b);
[[nodiscard]] f32 dist_sq(Vec3 a, Vec3 b);
[[nodiscard]] f32 dist_sq(Vec4 a, Vec4 b);

[[nodiscard]] f32 dist(Vec2 a, Vec2 b);
[[nodiscard]] f32 dist(Vec3 a, Vec3 b);
[[nodiscard]] f32 dist(Vec4 a, Vec4 b);

[[nodiscard]] f32 len_sq(Vec2 v);
[[nodiscard]] f32 len_sq(Vec3 const &v);
[[nodiscard]] f32 len_sq(Vec4 const &v);

[[nodiscard]] f32 Len(Vec2 v);
[[nodiscard]] f32 Len(Vec3 const &v);
[[nodiscard]] f32 Len(Vec4 const &v);

Vec2& normalize(Vec2 &v);
Vec3& normalize(Vec3 &v);
Vec4& normalize(Vec4 &v);

[[nodiscard]] Vec2 normalized(Vec2 v);
[[nodiscard]] Vec3 normalized(Vec3 v);
[[nodiscard]] Vec4 normalized(Vec4 v);

[[nodiscard]] f32 dot(Vec2 a, Vec2 b);
[[nodiscard]] f32 dot(Vec3 a, Vec3 b);
[[nodiscard]] f32 dot(Vec4 a, Vec4 b);

/**
 * Operators
*/

[[nodiscard]] bool operator==(Vec2 a, Vec2 b);
[[nodiscard]] Vec2 operator+(Vec2 a, Vec2 b);
[[nodiscard]] Vec2 operator-(Vec2 a, Vec2 b);
[[nodiscard]] Vec2 operator*(Vec2 a, Vec2 b);
[[nodiscard]] Vec2 operator/(Vec2 a, Vec2 b);
[[nodiscard]] Vec2 operator*(Vec2 v, f32 x);
Vec2& operator+=(Vec2 &a, Vec2 b);
Vec2& operator-=(Vec2 &a, Vec2 b);
Vec2& operator*=(Vec2 &a, Vec2 b);
Vec2& operator/=(Vec2 &a, Vec2 b);
Vec2& operator*=(Vec2 &a, f32 x);


[[nodiscard]] bool operator==(Vec3 a, Vec3 b);
[[nodiscard]] Vec3 operator+(Vec3 a, Vec3 b);
[[nodiscard]] Vec3 operator-(Vec3 a, Vec3 b);
[[nodiscard]] Vec3 operator*(Vec3 a, Vec3 b);
[[nodiscard]] Vec3 operator/(Vec3 a, Vec3 b);
[[nodiscard]] Vec3 operator*(Vec3 v, f32 x);
Vec3& operator+=(Vec3 &a, Vec3 b);
Vec3& operator-=(Vec3 &a, Vec3 b);
Vec3& operator*=(Vec3 &a, Vec3 b);
Vec3& operator/=(Vec3 &a, Vec3 b);
Vec3& operator*=(Vec3 &a, f32 x);


[[nodiscard]] bool operator==(Vec4 a, Vec4 b);
[[nodiscard]] Vec4 operator+(Vec4 a, Vec4 b);
[[nodiscard]] Vec4 operator-(Vec4 a, Vec4 b);
[[nodiscard]] Vec4 operator*(Vec4 a, Vec4 b);
[[nodiscard]] Vec4 operator/(Vec4 a, Vec4 b);
[[nodiscard]] Vec4 operator*(Vec4 v, f32 x);
Vec4& operator+=(Vec4 &a, Vec4 b);
Vec4& operator-=(Vec4 &a, Vec4 b);
Vec4& operator*=(Vec4 &a, Vec4 b);
Vec4& operator/=(Vec4 &a, Vec4 b);
Vec4& operator*=(Vec4 &a, f32 x);
} // namespace rt