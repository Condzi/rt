/**
 * Functions used for interfacing with debugger (if attached)
 */

namespace rt {
[[nodiscard]] bool
os_is_debugger_present();

void
os_print_to_debugger(char const *msg);

void
os_put_breakpoint_here();
} // namespace rt