//
// Common types declaration
//

typedef float4 Vec4;
typedef float3 Vec3;
typedef float2 Vec2;
typedef float  f32;
typedef uint   u32;
typedef int    s32;

// HLSL does not have infinity constant.
//
static const float INFINITY = 1. / 0.;

// @ToDo: This should be in the constant buffer
#define IMG_WIDTH 512
#define IMG_HEIGHT 512

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
};

Ray make_ray(Vec3 o, Vec3 dir) {
  Ray r;
  r.origin = o;
  r.direction = dir;

  return r;
}

Vec3
at(Ray r, f32 t) {
  return r.origin + r.direction * t;
}

struct Hit_Info {
  Material_ID mat_id;
  Vec3        p;
  Vec3        normal;
  f32         t;
  bool        front_face;
};

// empty hit info struct for correctness
Hit_Info make_hit_info() {
  Hit_Info hi;
  hi.mat_id = 0;
  hi.p = Vec3(0,0,0);
  hi.normal = Vec3(0,0,0);
  hi.t = 0;
  hi.front_face = false;

  return hi;
}

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
  Vec3 cam_origin;
  Vec3 cam_horizontal;
  Vec3 cam_vertical;
  Vec3 cam_lower_left_corner;
  Vec3 cam_u, cam_v, cam_w;
  f32  cam_lens_radius;
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
  f32 sn = fmod(dt, 3.14159265359);
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
  Vec3 pt = random_vec3();
  //while (len_sq(pt) >= 1) {
  //  pt = random_vec3();
  //}
  return pt;
}

Vec3 random_unit_vector() {
  return normalize(random_in_unit_sphere());
}

Vec3 random_in_unit_disk() {
  Vec3 p = Vec3(random_f32_in_range(Vec2(-1, 1)), random_f32_in_range(Vec2(-1, 1)), 0);
  //while (len_sq(p) >= 1) {
  //  p = Vec3(random_f32_in_range(Vec2(-1, 1)), random_f32_in_range(Vec2(-1, 1)), 0);
  //}
  return p;
}

//
//  Camera
//

Ray
get_ray_at(f32 s, f32 t) {
  /*
  const Vec3 rd     = random_in_unit_disk() * cam_lens_radius;
  const Vec3 offset = cam_u * rd.x + cam_v * rd.y;

  const Vec3 origin    = cam_origin + offset;
  const Vec3 direction = cam_lower_left_corner + cam_horizontal * s +
                         cam_vertical * t - cam_origin - offset;

  return make_ray(origin, direction);
  */

  Vec3 r_origin = cam_origin;
  Vec3 r_dir = cam_lower_left_corner + (s * cam_horizontal) + (t * cam_vertical) - r_origin;
  return make_ray(r_origin, r_dir);
}

//
//  Materials
//

// Helper math functions
Vec3 reflect(Vec3 v, Vec3 n) {
  return v - n * 2 * dot(v, n);
}

Vec3 refract(Vec3 uv, Vec3 n, f32 etai_over_etat) {
  f32 cos_theta = min(dot(-uv, n), 1.0);
  Vec3 r_out_perp = (uv + n * cos_theta) * etai_over_etat;
  Vec3 r_out_parallel = n * (-sqrt(abs(1.0 - dot(r_out_perp, r_out_perp))));

  return r_out_perp + r_out_parallel;
}

f32 reflectance(f32 cosine, f32 refraction_index) {
  f32 r0 = (1 - refraction_index) / (1 + refraction_index);
  r0 = r0 * r0;

  return r0 + (1 - r0) * pow((1 - cosine), 5);
}

/**
 * Specializations for different materials
 */

bool scatter_lambertian(const Material material,
                        const Hit_Info hi,
                        out Vec3 attenuation_color,
                        out Ray out_ray) {
  Vec3 scatter_direction = hi.normal + random_unit_vector();
  //if (near_zero(scatter_direction)) {
  if (len_sq(scatter_direction) < 0.001f) {
    scatter_direction = hi.normal;
  }

  out_ray = make_ray(hi.p, scatter_direction);
  attenuation_color = material.albedo;

  return true;
}

bool scatter_metal(const Material material,
                   const Ray in_ray,
                   const Hit_Info hi,
                   out Vec3 attenuation_color,
                   out Ray out_ray) {
  Vec3 reflected = reflect(normalize(in_ray.direction), hi.normal);
  Vec3 direction = reflected + random_in_unit_sphere() * material.fuzz;
  out_ray = make_ray(hi.p, direction);

  attenuation_color = material.albedo;
  return (dot(out_ray.direction, hi.normal) > 0);
}

bool scatter_dielectric(const Material material,
                        const Ray in_ray,
                        const Hit_Info hi,
                        out Vec3 attenuation_color,
                        out Ray out_ray) {
  f32 refraction_ratio = hi.front_face ? (1.0f / material.refraction_index)
                                       : material.refraction_index;

  Vec3 unit_direction = normalize(in_ray.direction);
  f32 cos_theta = min(dot(-unit_direction, hi.normal), 1.0);
  f32 sin_theta = sqrt(1.0f - cos_theta * cos_theta);

  bool can_refract = (refraction_ratio * sin_theta) <= 1.0f;
  bool reflectance_test = reflectance(cos_theta, refraction_ratio) > random_f32();

  Vec3 direction;
  if (can_refract && !reflectance_test) {
    direction = refract(unit_direction, hi.normal, refraction_ratio);
  } else {
    direction = reflect(unit_direction, hi.normal);
  }

  out_ray = make_ray(hi.p, direction);
  attenuation_color = float3(1.0f, 1.0f, 1.0f);

  return true;
}

Vec3
emit_diffuse_light(const Material material) {
  return material.albedo;
}


// Actual scatter and emit definitions
bool scatter(const Material material,
             const Ray in_ray,
             const Hit_Info hi,
             out Vec3 attenuation_color,
             out Ray out_ray) {
  attenuation_color = Vec3(0,0,0);
  out_ray.origin = Vec3(0,0,0);
  out_ray.direction = Vec3(0,0,0);

  switch (material.type) {
    case MaterialType_Lambertian: {
      return scatter_lambertian(material, hi, attenuation_color, out_ray);
    } break;

    case MaterialType_Metal: {
      return scatter_metal(material, in_ray, hi, attenuation_color, out_ray);
    } break;

    case MaterialType_Dielectric: {
      return scatter_dielectric(material, in_ray, hi, attenuation_color, out_ray);
    } break;

    default: {
    } break;
  }
  return false;
}

Vec3
emit(const Material material) {
  if (material.type == MaterialType_Diffuse_Light) {
    return emit_diffuse_light(material);
  }

  return Vec3(0, 0, 0);
}

//
//  Ray intersection detection
//
bool
hit_sphere(const Ray r, const Sphere s, f32 t_min, f32 t_max, out Hit_Info hi) {
  Vec3 oc = r.origin - s.center;

  // Inline len_sq and dot
  // Hot \/\/
  f32 a = r.direction.x * r.direction.x + r.direction.y * r.direction.y +
          r.direction.z * r.direction.z;
  f32 half_b = oc.x * r.direction.x + oc.y * r.direction.y + oc.z * r.direction.z;
  f32 c      = oc.x * oc.x + oc.y * oc.y + oc.z * oc.z - s.radius * s.radius;
  f32 discriminant = half_b * half_b - a * c;
  // ^^ hot

  f32 sd = sqrt(discriminant);

  // Optimized root calculation
  f32 root1 = (-half_b - sd) / a;
  f32 root2 = (-half_b + sd) / a;
  f32 root  = (root1 < t_max && root1 > t_min)   ? root1
              : (root2 < t_max && root2 > t_min) ? root2
                                                 : -1;

  if (root == -1) return false;

  hi.t = root;
  hi.p = at(r, root);

  f32  inv_radius     = 1 / s.radius;
  Vec3 outward_normal = (hi.p - s.center) * inv_radius;

  hi.front_face = (dot(r.direction, outward_normal) < 0);
  hi.normal     = outward_normal * (hi.front_face ? 1.f : -1.f);
  hi.mat_id     = s.mat_id;

  return true;
}

//
//  RT Logic
//

bool hit_scene(const Ray r, float tmin, float tmax, out Hit_Info hi) {
  // check if sphere is hit
  Hit_Info temp = make_hit_info();
  bool hit_ = false;
  float current_closest = tmax;

  hi = temp;
  [loop]
  for (int i = 0; i < num_spheres; i++) {
    if (hit_sphere(r, spheres[i], tmin, current_closest, temp)) {
      hit_ = true;
      current_closest = temp.t;
      hi = temp;
    }
  }
  return hit_;
}

Vec3 ray_color(const Ray r_in, int depth) {
  const Vec3 background_color = Vec3(0.5f, 0.7f, 1.0f);
  Hit_Info hi = make_hit_info();
  Vec3 final_color = Vec3(1, 1, 1);
  Ray current_ray = r_in;

  for (int i = 0; i < depth; ++i) {
    if (!hit_scene(r_in, 0.001f, 99999.f, hi)) {
      return background_color;
    }

    Ray scattered = r_in;
    Vec3 attenuated_color = Vec3(0,0,0);
    if (!scatter(materials[hi.mat_id], current_ray, hi, attenuated_color, scattered)) {
      return background_color;
    }

    current_ray = scattered;

    Vec3 color_from_scatter = attenuated_color*final_color;
    final_color = color_from_scatter;
  }

  return final_color;
}

[numthreads(16, 16, 1)]
void CSMain (uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID)
{
  uint2 id = uint2(DTid.x, DTid.y); // 2D index for 2D texture
  output[id] = float4(id.x/512.f, id.y/512.f, 1, 1);

  f32 u = f32(id.x);
  f32 v = f32(id.y);
  Ray r       = get_ray_at(u, v);
  Vec3 pixel_color = ray_color(r, 1);

//  pixel_color = pixel_color * (1.f / num_samples);
  pixel_color = Vec3(sqrt(pixel_color.r),
                     sqrt(pixel_color.g),
                     sqrt(pixel_color.b));
  pixel_color = saturate(pixel_color);

  output[id] = Vec4(pixel_color, 1);
}
