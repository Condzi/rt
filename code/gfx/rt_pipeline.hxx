namespace rt {
struct GFX_RT_Input {
  Vec2                 im_size;
  struct World const  &w;
  struct Camera const &c;
  // @ToDo: use BVH for broad-phase. We're not doing it yet
  // because BVH does not expose API for its internal lookup
  // arrays.
};

// @Note: this function is called after creating the RT world, not
// inside the gfx_init.
//
void
gfx_rt_init_or_panic(GFX_RT_Input const &in);

void
gfx_rt_start();

// Returns true if GPU finished ray tracing the scene.
//
[[nodiscard]] bool
gfx_rt_done();

[[nodiscard]] void*
gfx_rt_output_as_imgui_texture();
} // namespace rt
