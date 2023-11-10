
namespace rt {
struct Rt_Output {
  Vec2   image_size;
  Buffer rgba_data;
  s32               num_threads;
  std::atomic_bool *thread_flags; // true if thread finished
};

static std::atomic<s64> total_ray_count{0};

[[nodiscard]] Rt_Output
do_ray_tracing();
} // namespace rt

#include "cpu_materials.hxx"
