#include "d3d_layer.hxx"

namespace rt {
struct D3d {
  ::IDevice           *device             = NULL;
  ::IDeviceContext    *device_context     = NULL;
  ::ISwapChain        *swap_chain         = NULL;
  ::IRenderTargetView *render_target_view = NULL;

  struct XXC_Pipeline *xxc_pipeline = NULL;
  struct RT_Pipeline  *rt_pipeline = NULL;
} static gD3d;

void
gfx_init_or_panic() {
  ::DXGI_SWAP_CHAIN_DESC const swap_chain_descr = {
      .BufferDesc =
          {
              .RefreshRate = {.Numerator = 0, .Denominator = 1},
              .Format      = DXGI_FORMAT_B8G8R8A8_UNORM,
          },

      .SampleDesc = {.Count = 1, .Quality = 0},

      .BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT,
      .BufferCount  = 1,
      .OutputWindow = gWin32_Window.hWnd,
      .Windowed = true, // @Note: for fullscreen, set to false & set width and height
      .SwapEffect = DXGI_SWAP_EFFECT_DISCARD};

  ::UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;

#if HANDMADE_INTERNAL
  flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  ::D3D_FEATURE_LEVEL       feature_level;
  ::D3D_FEATURE_LEVEL const feature_levels[] = {D3D_FEATURE_LEVEL_11_1};
  ::HRESULT                 hr;
  hr = ::D3D11CreateDeviceAndSwapChain(NULL,
                                       D3D_DRIVER_TYPE_HARDWARE,
                                       NULL,
                                       flags,
                                       feature_levels,
                                       ARRAYSIZE(feature_levels),
                                       D3D11_SDK_VERSION,
                                       &swap_chain_descr,
                                       &gD3d.swap_chain,
                                       &gD3d.device,
                                       &feature_level,
                                       &gD3d.device_context);

  check_(gD3d.swap_chain);
  check_(gD3d.device);
  check_(gD3d.device_context);
  d3d_check_hresult_(hr);

  ::ITexture2D *framebuffer;
  hr = gD3d.swap_chain->GetBuffer(0, __uuidof(ITexture2D), (void **)&framebuffer);
  d3d_check_hresult_(hr);

  hr = gD3d.device->CreateRenderTargetView(framebuffer, 0, &gD3d.render_target_view);
  d3d_check_hresult_(hr);

  d3d_safe_release_(framebuffer);

  gfx_im_init_or_panic();
}

void
gfx_render() {
  /* clear the back buffer to cornflower blue for the new frame */
  float background_colour[4] = {0x64 / 255.0f, 0x95 / 255.0f, 0xED / 255.0f, 1.0f};
  gD3d.device_context->ClearRenderTargetView(gD3d.render_target_view,
                                             background_colour);

  // Tell the output merger to use our render target
  gD3d.device_context->OMSetRenderTargets(1, &gD3d.render_target_view, NULL);

  //gfx_im_flush();
  dear_imgui_draw();

  // Present
  gD3d.swap_chain->Present(VSYNC ? 1 : 0, 0);
}
} // namespace rt

#include "im_pipeline.cxx"
#include "rt_pipeline.cxx"
