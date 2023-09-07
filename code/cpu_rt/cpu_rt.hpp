
namespace rt {
struct Rt_Output {
  Vec2   image_size;
  Buffer rgba_data;
  s32               num_threads;
  std::atomic_bool *thread_flags; // true if thread finished
};

// Ray can be thought of as a function:
//  P(t) = A + t*B
struct Ray {
  Vec3 origin;
  Vec3 direction;
  Vec3 direction_inv;
};

struct Material;
struct Sphere {
  Vec3 center;
  f32  radius; // @Note: can be negative: surface normals will point inward.
  AABB aabb;
  // Move this somewhere else?
  Material *material;
};

[[nodiscard]] Rt_Output
do_raytraycing();
} // namespace rt

#include "camera.hxx"
#include "materials.hxx"
#include "bvh.hxx"
