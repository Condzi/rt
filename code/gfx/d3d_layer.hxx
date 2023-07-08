/**
 * Typedefs of ID3D types to avoid interface versioning.
 * D3D have various updates with increasing numbers as sufixes
 * to help in maintaining code.
 */

using IDevice           = ::ID3D11Device;
using IDeviceContext    = ::ID3D11DeviceContext;
using ISwapChain        = ::IDXGISwapChain;
using IRenderTargetView = ::ID3D11RenderTargetView;
using ITexture2D        = ::ID3D11Texture2D;
using IRasterizerState  = ::ID3D11RasterizerState;
using IVertexShader     = ::ID3D11VertexShader;
using IPixelShader      = ::ID3D11PixelShader;
using IInputLayout      = ::ID3D11InputLayout;
using IBuffer           = ::ID3D11Buffer;
using IBlob             = ::ID3DBlob;

#define d3d_safe_release_(x)                                                         \
  do {                                                                               \
    if (x) {                                                                         \
      x->Release();                                                                  \
      x = NULL;                                                                      \
    }                                                                                \
  } while (false)

#define d3d_check_hresult_(hr)                                                       \
  do {                                                                               \
    if (FAILED(hr)) {                                                                \
      String const error_str = os_error_to_string(hr);                               \
                                                                                     \
      logf("[D3D] !!! %s:%d -- ", __FILE__, (s32)__LINE__);                          \
      logf("%.*s\n", (s32)error_str.count, error_str.data);                          \
                                                                                     \
      errf("Failed to initialize DirectX 11.");                                      \
    }                                                                                \
  } while (false)
