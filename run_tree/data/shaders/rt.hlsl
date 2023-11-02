//
// Common types declaration
//

typedef float4 Vec4;
typedef float3 Vec3;
typedef float2 Vec2;
typedef float  f32;
typedef uint   u32;
typedef int    s32;

//
//  MATERIALS
//

#define MaterialType_None 0
#define MaterialType_Lambertian 1
#define MaterialType_Metal 2
#define MaterialType_Dielectric 3
#define MaterialType_Diffuse_Light 4

typedef u32 Material_ID;

struct Material {
  u32  type;
  Vec3 albedo;           // Common among multiple material types
  f32  fuzz;             // Only used in Metal
  f32  refraction_index; // Only used in Dielectric
};

//
//  SHAPES
//

struct Sphere {
  Vec3 center;
  f32  radius;

  Material_ID mat_id;
};

struct Quad {
  Vec3 normal;
  f32 D;
  Vec3 w;

  Material_ID mat_id;
};

//
//  Ray and Hit_Info
//
struct Ray {
  Vec3 origin;
  Vec3 direction;
  Vec3 direction_inv;
};

Ray make_ray(Vec3 o, Vec3 dir) {
  Ray r;
  r.origin = o;
  r.direction = dir;
  r.direction_inv = 1/dir;

  return r;
}

struct Hit_Info {
  Material_ID mat_id;
  Vec3        p;
  Vec3        normal;
  f32         t;
  bool        front_face;
};

// Declare the UAV for output.
//
RWTexture2D<float4> output : register(u0);

cbuffer ConstantBuffer : register(b0)
{
  // Quality
  //
  s32 num_samples;
  s32 num_reflections;

  // Buffers sizes
  //
  s32 num_spheres;
  s32 num_quads;

  // Camera properties
  //
  Vec3 origin;
  Vec3 horizontal;
  Vec3 vertical;
  Vec3 lower_left_corner;
  Vec3 u, v, w;
  f32  lens_radius;
}

StructuredBuffer<Sphere>   spheres   : register(t0);
// StructuredBuffer<Quad>     quads     : register(t1);
StructuredBuffer<Material> materials : register(t2);

//
//  RNG
//

// https://github.com/D-K-E/raytracing-gl/blob/master/bin/media/shaders/compute07.comp#L18C1-L26C2
f32 random_f32_in_range(Vec2 co) {
  // random gen
  f32 a = 12.9898;
  f32 b = 78.233;
  f32 c = 43758.5453;
  f32 dt = dot(co.xy, Vec2(a, b));
  f32 sn = fmod(dt, 3.14159265359); // PI in HLSL
  return frac(sin(sn) * c);
}

f32 random_f32() {
  return random_f32_in_range(Vec2(0, 1));
}

s32 random_s32_in_range(s32 min, s32 max) {
  return (s32)random_f32_in_range(Vec2((f32)min, (f32)max));
}

Vec3 random_vec3() {
  return float3(random_f32(), random_f32(), random_f32());
}

Vec3 random_vec3_in_range(f32 min, f32 max) {
  return random_vec3() * (max - min) + float3(min, min, min);
}

f32 len_sq(Vec3 v) {
  return dot(v, v);
}

Vec3 random_in_unit_sphere() {
  [loop]
  while (true) {
    Vec3 pt = random_vec3();
    if (len_sq(pt) >= 1) {
      continue;
    }
    return pt;
  }
}

Vec3 random_unit_vector() {
  return normalize(random_in_unit_sphere());
}

Vec3 random_in_hemisphere(Vec3 normal) {
  Vec3 in_unit_sphere = random_in_unit_sphere();
  f32 d = dot(in_unit_sphere, normal);
  return (d > 0.0) ? in_unit_sphere : -in_unit_sphere;
}

Vec3 random_in_unit_disk() {
  [loop]
  while (true) {
    Vec3 p = float3(random_f32_in_range(Vec2(-1, 1)), random_f32_in_range(Vec2(-1, 1)), 0);
    if (len_sq(p) >= 1) {
      continue;
    }
    return p;
  }
}


[numthreads(16, 16, 1)]
void CSMain (uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID)
{
  uint2 id = uint2(DTid.x, DTid.y); // 2D index for 2D texture
  output[id] = float4(1, 1, 1, 1);
}
