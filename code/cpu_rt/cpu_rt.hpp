
namespace rt {
struct Rt_Output {
  Vec2   image_size;
  Buffer rgba_data;
  s32               num_threads;
  std::atomic_bool *thread_flags; // true if thread finished
};

enum Object_Type : u8 { ObjectType_None = 0, ObjectType_Sphere, ObjectType_Quad };

// @Note: maybe pack it into 32 bits?
union Object_ID {
  struct {
    u32         idx;
    Object_Type type;
  };
  u64 i;
};

Object_ID constexpr static INVALID_OBJECT_ID {.i = 0};

// Ray can be thought of as a function:
//  P(t) = A + t*B
struct Ray {
  Vec3 origin;
  Vec3 direction;
  Vec3 direction_inv;
};

static std::atomic<s64> total_ray_count{0};

[[nodiscard]] Ray
make_ray(Vec3 origin, Vec3 direction);

[[nodiscard]] Rt_Output
do_ray_tracing();
} // namespace rt

#include "camera.hxx"
#include "shapes.hxx"
#include "materials.hxx"
#include "bvh.hxx"
#include "world.hxx"
