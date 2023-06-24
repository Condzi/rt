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
    logf("[D3D] !!! %s:%d -- ", __FILE__, (s32)__LINE__)            \                                                    \
    logf("%.*s\n", (s32)error_str.count, error_str.data);           \
  }                                                                 \
} while(false)

namespace rt {

struct D3d {
  ID3D11Device           *device             = NULL;
  ID3D11DeviceContext    *device_context     = NULL;
  IDXGISwapChain         *swap_chain         = NULL;
  ID3D11RenderTargetView *render_target_view = NULL; 
} static gD3d;
} // namespace rt