// base_rt stores common code between GFX RT and CPU RT.
//
namespace rt {
// Ray can be thought of as a function:
//  P(t) = A + t*B
struct Ray {
  Vec3 origin;
  Vec3 direction;
};
[[nodiscard]] Ray
make_ray(Vec3 origin, Vec3 direction);

[[nodiscard]] Vec3
ray_at(Ray const &r, f32 t);

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

} // namespace rt

#include "camera.hxx"
#include "materials.hxx"
#include "shapes.hxx"
#include "bvh.hxx"
#include "world.hxx"
