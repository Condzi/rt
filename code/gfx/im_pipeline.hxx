namespace rt {
void
gfx_im_init_or_panic();

void
gfx_im_vertex(Vec2 position, u32 color);

void
gfx_im_rect(Vec2 position, Vec2 size, u32 color);

void
gfx_im_flush();
} // namespace rt