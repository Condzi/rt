namespace rt {
u32 constexpr COLOR_RED   = 0xff0000ff;
u32 constexpr COLOR_GREEN = 0x00ff00ff;
u32 constexpr COLOR_BLUE  = 0x0000ffff;

void
gfx_init_or_panic();

void
gfx_render();
} // namespace rt

#include "im_pipeline.hxx"
#include "rt_pipeline.hxx"
