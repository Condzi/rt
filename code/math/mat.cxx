namespace rt {
[[nodiscard]] Mat4x4 identity() {
	Mat4x4 m{};
	m.v[0][0] = 1.f;
	m.v[1][1] = 1.f;
	m.v[2][2] = 1.f;
	m.v[3][3] = 1.f;

	return m;
}

[[nodiscard]] Vec4 transformed_point(Vec4 const &p, Mat4x4 const &m) {
	auto const &M = m.v;
	return {
    .x = p.x*M[0][0] + p.y*M[1][0] + p.z*M[2][0] + p.w*M[3][0],
    .y = p.x*M[0][1] + p.y*M[1][1] + p.z*M[2][1] + p.w*M[3][1],
    .z = p.x*M[0][2] + p.y*M[1][2] + p.z*M[2][2] + p.w*M[3][2],
    .w = p.x*M[0][3] + p.y*M[1][3] + p.z*M[2][3] + p.w*M[3][3] 
	};
}

[[nodiscard]] Vec2 transformed_point(Vec2 const &p, Mat4x4 const &m) {
	Vec4 V4 = transformed_point({p.x, p.y, 0.f, 1.f}, m);
	return V4.xy;
}

Vec4& transform_point(Vec4 &p, Mat4x4 const &m) {
	Vec4 Copy = p;
	p = transformed_point(p, m);
	return p;
}

Vec2& transform_point(Vec2 &p, Mat4x4 const &m) {
	Vec2 Copy = transformed_point(p, m);
	p = Copy;
	return p;
}

[[nodiscard]] Mat4x4 combine(Mat4x4 const &a, Mat4x4 const &b) {
	Mat4x4 Result{};
	auto &R = Result.v;
	auto const &A = a.v;
	auto const &B = b.v;

  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
        for (int i = 0; i < 4; i++) {
          R[r][c] += A[r][i]*B[i][c];
        }
    }
  }

	return Result;
}

[[nodiscard]] Mat4x4 transpose(Mat4x4 m) {
  f32* V = (f32*)&m.v;

  rt_swap(V[1],  V[4]);
  rt_swap(V[2],  V[8]);
  rt_swap(V[3],  V[12]);
  rt_swap(V[6],  V[9]);
  rt_swap(V[7],  V[13]);
  rt_swap(V[11], V[14]);
  return m;
}


[[nodiscard]] Mat4x4 rot_z(f32 angle) {
	f32 const S = std::sin(angle);
	f32 const C = std::cos(angle);

	return {{
    { C, S, 0, 0},
    {-S, C, 0, 0},
    { 0, 0, 1, 0},
    { 0, 0, 0, 1}
  }};
}


[[nodiscard]] Mat4x4 scale2(Vec2 factor) {
	f32 const x = factor.x;
	f32 const y = factor.y;
  return {{
    {x, 0, 0, 0},
    {0, y, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
  }};
}

[[nodiscard]] Mat4x4 translate2(Vec2 o) {
	f32 const x = o.x;
	f32 const y = o.y;
  return {{
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {x, y, 0, 1}
  }};
}

// top, left, bottom, right
// height, 0, 0, width
// top = height
// left = bottom = 0
// right = width
[[nodiscard]] Mat4x4 ortho_proj(f32 width, f32 height) {
	dbg_check_(width > 0);
	dbg_check_(height > 0);

  f32 const W =  width; 
  f32 const H =  height;
  f32 const A = -width; 
  f32 const B = -height;

  Mat4x4 Result{};
  auto& R = Result.v;

  R[0][0] = 2/W;
  R[1][1] = 2/H;
  R[2][2] = -1;
  R[3][0] = A/W;
  R[3][1] = B/H;
  R[3][3] = 1;

  return Result;
}
} // namespace rt