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
  // @todo
}

[[nodiscard]] Vec2
window_get_size() {
  // @todo
  return {.x = 0, .y = 0};
}

[[nodiscard]] bool
window_is_open() {
  // @todo
  return false;
}

[[nodiscard]] bool
window_is_closed() {
  // @todo
  return false;
}

[[nodiscard]] bool
window_is_minimized() {
  // @todo
  return false;
}

void
window_close() {
  // @todo
}
} // namespace rt