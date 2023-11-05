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
  Vec3 Q;
  Vec3 u,v;
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
  s32 num_materials;
  f32 cam_lens_radius;

  // Camera properties
  //
  Vec3 cam_origin;
  Vec3 cam_horizontal;
  Vec3 cam_vertical;
  Vec3 cam_lower_left_corner;
  Vec3 cam_u, cam_v, cam_w;
}

StructuredBuffer<Sphere>   spheres   : register(t0);
StructuredBuffer<Quad>     quads     : register(t1);
StructuredBuffer<Material> materials : register(t2);

//
//  RNG, xorshift-based
//

// RNG functions
uint Rand(inout uint rand_seed)
{
    uint x = rand_seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return rand_seed = x;
}

float random_f32(inout uint rand_seed)
{
    return Rand(rand_seed) / 4294967296.0;
}

float random_f32_in_range(inout uint rand_seed, float min, float max)
{
    return min + (max - min) * random_f32(rand_seed);
}

Vec3 random_vec3(inout uint rand_seed)
{
    return float3(random_f32(rand_seed), random_f32(rand_seed), random_f32(rand_seed));
}

Vec3 random_in_unit_sphere(inout uint rand_seed)
{
    Vec3 pt;
    do {
        pt = random_vec3(rand_seed);
    } while (length(pt) >= 1.0);
    return pt;
}

Vec3 random_unit_vector(inout uint rand_seed)
{
    return normalize(random_in_unit_sphere(rand_seed));
}

Vec3 random_in_unit_disk(inout uint rand_seed)
{
    Vec3 p;
    do {
        p = float3(random_f32_in_range(rand_seed, -1, 1), random_f32_in_range(rand_seed, -1, 1), 0);
    } while (length(p) >= 1.0);
    return p;
}

//
//  Math helpers
//

#define FLT_EPSILON      1.192092896e-07F        // smallest such that 1.0+FLT_EPSILON != 1.0
bool f32_compare(f32 a, f32 b) {
  return abs(a - b) < FLT_EPSILON;
}

bool near_zero(Vec3 v) {
  return f32_compare(v.x, 0) && f32_compare(v.y, 0) && f32_compare(v.z, 0);
}

//
//  Camera
//

Ray
get_ray_at(inout uint rand_seed, f32 s, f32 t) {
  const Vec3 rd     = random_in_unit_disk(rand_seed) * cam_lens_radius;
  const Vec3 offset = (cam_u * rd.x) + (cam_v * rd.y);

  const Vec3 origin    = cam_origin + offset;
  const Vec3 direction = cam_lower_left_corner + (cam_horizontal * s) +
                         (cam_vertical * t) - cam_origin - offset;

  return make_ray(origin, direction);
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

bool scatter_lambertian(inout uint rand_seed,
                        const Material material,
                        const Hit_Info hi,
                        out Vec3 attenuation_color,
                        out Ray out_ray) {
  Vec3 scatter_direction = hi.normal + random_unit_vector(rand_seed);
  if (near_zero(scatter_direction)) {
    scatter_direction = hi.normal;
  }

  out_ray = make_ray(hi.p, scatter_direction);
  attenuation_color = material.albedo;

  return true;
}

bool scatter_metal(inout uint rand_seed,
                   const Material material,
                   const Ray in_ray,
                   const Hit_Info hi,
                   out Vec3 attenuation_color,
                   out Ray out_ray) {
  Vec3 reflected = reflect(normalize(in_ray.direction), hi.normal);
  Vec3 direction = reflected + random_in_unit_sphere(rand_seed) * material.fuzz;
  out_ray = make_ray(hi.p, direction);

  attenuation_color = material.albedo;
  return (dot(out_ray.direction, hi.normal) > 0);
}

bool scatter_dielectric(inout uint rand_seed,
                        const Material material,
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
  bool reflectance_test = reflectance(cos_theta, refraction_ratio) > random_f32(rand_seed);

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
bool scatter(inout uint rand_seed,
             const Material material,
             const Ray in_ray,
             const Hit_Info hi,
             out Vec3 attenuation_color,
             out Ray out_ray) {
  attenuation_color = Vec3(0,0,0);
  out_ray.origin = Vec3(0,0,0);
  out_ray.direction = Vec3(0,0,0);

  switch (material.type) {
    case MaterialType_Lambertian: {
      return scatter_lambertian(rand_seed, material, hi, attenuation_color, out_ray);
    } break;

    case MaterialType_Metal: {
      return scatter_metal(rand_seed, material, in_ray, hi, attenuation_color, out_ray);
    } break;

    case MaterialType_Dielectric: {
      return scatter_dielectric(rand_seed, material, in_ray, hi, attenuation_color, out_ray);
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

bool
hit_quad(const Ray r, const Quad q, f32 t_min, f32 t_max, out Hit_Info hi) {
  const f32 denom = dot(q.normal, r.direction);

  // Ray is parallel to the plane
  if (abs(denom) < 1e-8) {
    return false;
  }

  // Intersection point is outside the ray interval
  const f32 t = (q.D - dot(q.normal, r.origin)) / denom;
  if (t < t_min || t > t_max) {
    return false;
  }

  const Vec3 intersection_point = at(r, t);
  // Determine the hit point lies within the planar shape using its plane coordinates.

  const Vec3 planar_hitpt_vector = intersection_point - q.Q;
  const f32  alpha               = dot(q.w, cross(planar_hitpt_vector, q.v));
  const f32  beta                = dot(q.w, cross(q.u, planar_hitpt_vector));

  // Check if we're inside the (0, 1) range
  if ((alpha < 0 || alpha > 1) || (beta < 0 || beta > 1)) {
    return false;
  }

  // Ray hits the 2D shape; set the hit record and return true.
  hi.t          = t;
  hi.p          = intersection_point;
  hi.mat_id     = q.mat_id;
  hi.front_face = (dot(r.direction, q.normal) < 0);
  hi.normal     = q.normal * (hi.front_face ? 1.f : -1.f);

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
  for (int i = 0; i < num_spheres; i++) {
    if (hit_sphere(r, spheres[i], tmin, current_closest, temp)) {
      hit_ = true;
      current_closest = temp.t;
      hi = temp;
    }
  }

  for (int i = 0; i < num_quads; i++) {
    if (hit_quad(r, quads[i], tmin, current_closest, temp)) {
      hit_ = true;
      current_closest = temp.t;
      hi = temp;
    }
  }

  return hit_;
}

Vec3 ray_color(inout uint rand_seed, Ray r_in, int depth) {
  const Vec3 background_color = Vec3(0.5, 0.7, 1.0);
  Vec3 final_color = Vec3(1,1,1);

  Ray current_ray = r_in;
  for (int i = 0; i < depth; i++) {
    Hit_Info hi = make_hit_info();
    if (!hit_scene(current_ray, 0.001, INFINITY, hi)) {
      return final_color * background_color;
    }

    Vec3 attenuated_color;
    Vec3 emitted_color = emit(materials[hi.mat_id]);

    Ray scattered_ray;
    if (!scatter(rand_seed, materials[hi.mat_id], current_ray, hi, attenuated_color, scattered_ray)) {
      return final_color*emitted_color;
    }

    current_ray = scattered_ray;
    final_color = (final_color*attenuated_color) + emitted_color;
  }

  return Vec3(0,0,0);
}

[numthreads(16, 16, 1)]
void CSMain (uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID) {
  // Seed cannot be 0!
  //
  uint rand_seed = DTid.x * IMG_WIDTH + DTid.y + 2137;

  uint2 id = uint2(DTid.x, DTid.y); // 2D index for 2D texture
  Vec3 pixel_color = Vec3(0,0,0);
  
  for (int i = 0; i < num_samples; i++) {
    f32 u = (f32(id.x) + random_f32(rand_seed))/(IMG_WIDTH - 1);
    f32 v = 1.0 - (f32(id.y) + random_f32(rand_seed))/(IMG_HEIGHT - 1);
    Ray r       = get_ray_at(rand_seed, u, v);
    pixel_color = pixel_color + ray_color(rand_seed, r, num_reflections);
  }

  pixel_color = pixel_color * (1.f / num_samples);
  pixel_color = Vec3(sqrt(pixel_color.r),
                     sqrt(pixel_color.g),
                     sqrt(pixel_color.b));
  pixel_color = clamp(pixel_color, 0.0f, 0.999f);

  output[id] = Vec4(pixel_color, 1);
}
