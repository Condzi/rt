#include "imgui_dx11.hxx"
#include "imgui_win32.hxx"

extern IMGUI_IMPL_API LRESULT
ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace rt {
void
dear_imgui_init() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui::StyleColorsDark();

  ImGui_ImplWin32_Init(gWin32_Window.hWnd);
  ImGui_ImplDX11_Init(gD3d.device, gD3d.device_context);
}

void
dear_imgui_update() {
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  /*
  bool show_demo_window = false;
  ImGui::ShowDemoWindow(&show_demo_window);
  */
}

void
dear_imgui_draw() {
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

// Our Y axis is inverted
void
flip_image(u8 *pixels, int width, int height) {
  s32 const bytes_per_pixel = 4;
  u8       *temp_row        = (u8 *)alloc_temp(width * bytes_per_pixel);

  for (int y = 0; y < height / 2; ++y) {
    // Index of the pixel at the top of the image
    int top_idx = y * width * bytes_per_pixel;

    // Index of the pixel at the bottom of the image
    int bot_idx = (height - y - 1) * width * bytes_per_pixel;

    // Swap rows
    mem_copy_(temp_row, &pixels[top_idx], width * bytes_per_pixel);
    mem_copy_(&pixels[top_idx], &pixels[bot_idx], width * bytes_per_pixel);
    mem_copy_(&pixels[bot_idx], temp_row, width * bytes_per_pixel);
  }
}

[[nodiscard]] ImTextureID
dear_imgui_create_texture_from_rt_output(CPU_RT_Output const &rt_out) {
  auto const  width  = (::UINT)rt_out.image_size.width;
  auto const  height = (::UINT)rt_out.image_size.height;
  void const *pixels = rt_out.rgba_data.bytes;
  s32 const   bytes_per_pixel = 4;
  u8         *temp_img        = (u8 *)alloc_temp(width * height * bytes_per_pixel);

  ::memcpy(temp_img, pixels, width * height * bytes_per_pixel);

  flip_image(temp_img, width, height);
  // Create the Direct3D texture
  D3D11_TEXTURE2D_DESC const desc {.Width      = width,
                                   .Height     = height,
                                   .MipLevels  = 1,
                                   .ArraySize  = 1,
                                   .Format     = DXGI_FORMAT_R8G8B8A8_UNORM,
                                   .SampleDesc = {.Count = 1},
                                   .Usage      = D3D11_USAGE_DEFAULT,
                                   .BindFlags  = D3D11_BIND_SHADER_RESOURCE};

  D3D11_SUBRESOURCE_DATA const sub_resource {.pSysMem     = temp_img,
                                             .SysMemPitch = desc.Width * 4};

  ::HRESULT        hr;
  ID3D11Texture2D *my_texture = nullptr;
  hr = gD3d.device->CreateTexture2D(&desc, &sub_resource, &my_texture);

  d3d_check_hresult_(hr);

  ID3D11ShaderResourceView *my_texture_srv = nullptr;
  hr = gD3d.device->CreateShaderResourceView(my_texture, NULL, &my_texture_srv);

  d3d_check_hresult_(hr);

  d3d_safe_release_(my_texture);

  // Shader Resource View -- texture ID in ImGui
  return my_texture_srv;
}

void
dear_imgui_delete_texture(ImTextureID texture) {
  auto view = ((ID3D11ShaderResourceView *)texture);

  d3d_safe_release_(view);
}

[[nodiscard]] bool
dear_imgui_window_proc(::LRESULT &lresult,
                       ::HWND     hwnd,
                       ::UINT     message,
                       ::WPARAM   wParam,
                       ::LPARAM   lParam) {

  lresult = ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam);
  return lresult != 0;
}
} // namespace rt

#include "3rdparty/imgui/imgui.cpp"
#include "3rdparty/imgui/imgui_demo.cpp"
#include "3rdparty/imgui/imgui_widgets.cpp"
#include "3rdparty/imgui/imgui_tables.cpp"
#include "3rdparty/imgui/imgui_draw.cpp"

#include "imgui_win32.cxx"
#include "imgui_dx11.cxx"
