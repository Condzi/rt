namespace rt {
void
win32_message_loop();

// General Win32 window procedure that calls other handlers
::LRESULT CALLBACK
win32_main_window_proc(::HWND hwnd, ::UINT message, ::WPARAM wParam, ::LPARAM lParam);

// Win32 window proc for handling window-related events.
// Returns false if the message was not handled by this handler. In this case, lresult
// is not modified.
[[nodiscard]] bool
win32_window_proc(::LRESULT &lresult,
                  ::HWND     hwnd,
                  ::UINT     message,
                  ::WPARAM   wParam,
                  ::LPARAM   lParam);
} // namespace rt