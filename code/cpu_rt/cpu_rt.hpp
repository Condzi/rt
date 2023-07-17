namespace rt {
struct Rt_Output {
  Vec2   image_size;
  Buffer rgba_data;
};

[[nodiscard]] Rt_Output
do_raytraycing();
} // namespace rt
