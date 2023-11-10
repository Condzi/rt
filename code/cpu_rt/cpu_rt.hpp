
namespace rt {
struct CPU_RT_Output {
  Vec2   image_size;
  Buffer rgba_data;
  s32               num_threads;
  std::atomic_bool *thread_flags; // true if thread finished
};

struct CPU_RT_Input {
  Vec2                         im_size;
  World const                 &w;
  Camera const                &c;
  std::vector<BVH_Flat> const &bvh;
};

static std::atomic<s64> total_ray_count {0};

[[nodiscard]] CPU_RT_Output
do_ray_tracing(CPU_RT_Input const &in);
} // namespace rt

#include "cpu_materials.hxx"
