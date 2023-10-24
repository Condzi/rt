//
// Common types declaration
//

typedef float4 Vec4;
typedef float3 Vec3;
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
StructuredBuffer<Quad>     quads     : register(t1);
StructuredBuffer<Material> materials : register(t2);

[numthreads(16, 16, 1)]
void CSMain (uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID)
{
  uint2 id = uint2(DTid.x, DTid.y); // 2D index for 2D texture
  output[id] = float4(1, 1, 1, 1);
}
