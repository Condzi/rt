namespace rt {
[[nodiscard]] bool
os_is_debugger_present() {
  return ::IsDebuggerPresent();
}

void
os_print_to_debugger(char const *msg) {
  // @Unicode
  ::OutputDebugStringA(msg);
}

void
os_put_breakpoint_here() {
  ::__debugbreak();
}
} // namespace rt