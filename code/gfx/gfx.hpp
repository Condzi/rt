#include "im_pipeline.hxx"

namespace rt {
u32 constexpr COLOR_RED   = 0xff0000ff;
u32 constexpr COLOR_GREEN = 0x00ff00ff;
u32 constexpr COLOR_BLUE  = 0x0000ffff;

// XX - x and y position
// C  - 32-bit color
struct Vertex_XXC {
  Vec2 position;
  u32  color;
};

struct alignas(16) Constants {
  Mat4x4 projection;
  Mat4x4 camera;
};

void
gfx_init_or_panic();

void
gfx_render();
} // namespace rt