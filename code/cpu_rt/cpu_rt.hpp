
namespace rt {
struct Rt_Output {
  Vec2   image_size;
  Buffer rgba_data;
};

struct Ray {
  Vec3 origin;
  Vec3 direction;
};

[[nodiscard]] Rt_Output
do_raytraycing();
} // namespace rt

#include "camera.hxx"
