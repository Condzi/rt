#include "im_pipeline.hxx"


#define d3d_safe_release_(x) \
do {                         \
  if (x) {                   \
    x->Release();            \
    x = NULL;                \
  }                          \
} while(false)

#define d3d_check_hresult_(hr)                                      \
do {                                                                \
  if (FAILED(hr)) {                                                 \
    String const error_str = os_error_to_string(hr);                \
                                                                    \
    logf("[D3D] !!! %s:%d -- ", __FILE__, (s32)__LINE__);           \
    logf("%.*s\n", (s32)error_str.count, error_str.data);           \
                                                                    \
    errf("Failed to initialize DirectX 11.");                       \
  }                                                                 \
} while(false)

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
} // namespace rt;