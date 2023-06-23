namespace rt {
void 
win32_message_loop() {
  MSG msg = {0};
  if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  } 
}

::LRESULT CALLBACK 
win32_main_window_proc(::HWND hwnd, ::UINT message, ::WPARAM wParam, ::LPARAM lParam) {
  bool was_message_handled = false;
  ::LRESULT result;

  was_message_handled |= win32_window_proc(result, hwnd, message, wParam, lParam);
  // ... more here ...

  if (!was_message_handled) {
    result = ::DefWindowProc(hwnd, message, wParam, lParam);
  }

  return result;
}

[[nodiscard]] bool
win32_window_proc(::LRESULT &lresult, 
                  ::HWND hwnd, ::UINT message, ::WPARAM wParam, ::LPARAM lParam) {
  bool handled = true;

  switch (message) {
    case WM_SYSCOMMAND: {
      switch (wParam) {
        // @Note: this should be handled by the debug renderer too - 
        //        don't render if we're not visible. However, the RT
        //        should still take place, since it's taking a bit...
        case SC_MINIMIZE: {
          gWin32_Window.mode = WindowMode_Minimized;
          logf("win32_window_proc: MINIMIZED\n");
        } break;

        case SC_MAXIMIZE: {
          gWin32_Window.mode = WindowMode_Open;
          logf("win32_window_proc: MAXIMIZED\n");
        } break;

        case SC_RESTORE: {
          gWin32_Window.mode = WindowMode_Open;
          logf("win32_window_proc: RESTORED\n");
        } break;
      }
      lresult = ::DefWindowProc(hwnd, message, wParam, lParam);
    } break;

    case WM_CLOSE: {
      ::DestroyWindow(hwnd);
      gWin32_Window.mode = WindowMode_Closed;
      logf("win32_window_proc: WM_CLOSE\n");
      lresult = 0;
    } break;

    case WM_DESTROY: {
      ::PostQuitMessage(0);
      lresult = 0;
      logf("win32_window_proc: WM_DESTROY\n");
    } break;

    default: {
      handled = false;
    } break;
  }

  return handled;
}
} // namespace rt