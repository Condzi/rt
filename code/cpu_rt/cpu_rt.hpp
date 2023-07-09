namespace rt {
struct Rt_Output {
  Vec2   image_size;
  Buffer rgb_data;
};

[[nodiscard]] Rt_Output
do_raytraycing();
} // namespace rt
