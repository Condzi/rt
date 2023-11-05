// Enable discrite GPU
//
__declspec(dllexport) DWORD NvOptimusEnablement                = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

/**
 * Typedefs of ID3D types to avoid interface versioning.
 * D3D have various updates with increasing numbers as sufixes
 * to help in maintaining code.
 */

using IDevice              = ::ID3D11Device;
using IDeviceContext       = ::ID3D11DeviceContext;
using ISwapChain           = ::IDXGISwapChain;
using IRenderTargetView    = ::ID3D11RenderTargetView;
using ITexture2D           = ::ID3D11Texture2D;
using IRasterizerState     = ::ID3D11RasterizerState;
using IComputeShader       = ::ID3D11ComputeShader;
using IVertexShader        = ::ID3D11VertexShader;
using IPixelShader         = ::ID3D11PixelShader;
using IInputLayout         = ::ID3D11InputLayout;
using IBuffer              = ::ID3D11Buffer;
using IBlob                = ::ID3DBlob;
using IUnorderedAccessView = ::ID3D11UnorderedAccessView;
using IShaderResourceView  = ::ID3D11ShaderResourceView;
using IQuery               = ::ID3D11Query;

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
      HRESULT rem_reason = gD3d.device->GetDeviceRemovedReason();                    \
      errf("Failed to initialize DirectX 11. Device removed reason: 0x%X",           \
           (int)rem_reason);                                                         \
    }                                                                                \
  } while (false)
