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

  bool show_demo_window = true;
  ImGui::ShowDemoWindow(&show_demo_window);
}

void
dear_imgui_draw() {
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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
