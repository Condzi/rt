#include "wnd_proc.hxx"

namespace rt {
enum Window_Mode {
  WindowMode_Closed = 0,
  WindowMode_Open,
  WindowMode_Minimized
};

struct Win32_Window {
  ::HWND hWnd;
  // @Note: Updated by the window proc in order to avoid calls to WinApi
  Window_Mode mode; 
} static gWin32_Window;

void
window_create_or_panic() {
  WNDCLASSEXA window_class = {
    .cbSize        = sizeof(WNDCLASSEX),
    .style         = CS_HREDRAW | CS_VREDRAW,
    .lpfnWndProc   = win32_main_window_proc,
    .hInstance     = ::GetModuleHandleA(NULL),
    .hCursor       = ::LoadCursorA(NULL, IDC_ARROW),
    .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
    .lpszMenuName  = NULL,
    .lpszClassName = WIN_CLASS
  };

  if (!::RegisterClassExA(&window_class)) {
    errf("Failed to register window class");
  }
  
  UINT constexpr window_style = WS_OVERLAPPEDWINDOW;
  RECT           window_rect  = {0, 0, WIN_WIDTH, WIN_HEIGHT};
  ::AdjustWindowRect(&window_rect, window_style, FALSE);

  gWin32_Window.hWnd = ::CreateWindowA(WIN_CLASS, 
                                       WIN_NAME, 
                                       window_style, 
                                       CW_USEDEFAULT, 
                                       CW_USEDEFAULT, 
                                       window_rect.right  - window_rect.left, 
                                       window_rect.bottom - window_rect.top, 
                                       NULL, 
                                       NULL, 
                                       GetModuleHandleA(NULL), 
                                       NULL
                                       );

  if (!gWin32_Window.hWnd) {
    errf("Failed to create window");
  }

  ::ShowWindow(gWin32_Window.hWnd, 1);
  ::UpdateWindow(gWin32_Window.hWnd);

  gWin32_Window.mode = WindowMode_Open;
}

[[nodiscard]] Vec2
window_get_size() {
  ::RECT rect = {};

  check_(::GetClientRect(gWin32_Window.hWnd, &rect));

  Vec2 const size = {
    .width  = (f32)rect.right  - rect.left,
    .height = (f32)rect.bottom - rect.top
  };

  return size;
}

[[nodiscard]] bool
window_is_open() {
  return gWin32_Window.mode == WindowMode_Open;
}

[[nodiscard]] bool
window_is_closed() {
  return gWin32_Window.mode == WindowMode_Closed;
}

[[nodiscard]] bool
window_is_minimized() {
  return gWin32_Window.mode == WindowMode_Minimized;
}

void
window_close() {
  ::PostMessageA(gWin32_Window.hWnd, WM_CLOSE, 0, 0);
}
} // namespace rt

// @Note: needs the definitions from above
#include "wnd_proc.cxx"