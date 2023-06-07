namespace rt {
/**
 * Error handling
*/
[[nodiscard]] String
os_error_to_string(u32 error_code);

[[nodiscard]] u32
os_get_last_error();

// @Note: VEH is added only when debugger is not present because otherwise we trap
//        debugger related exceptions.
void 
os_register_vectored_exception_handler();

/**
 * Debugging
*/

[[nodiscard]] bool
os_is_debugger_present() {
  // @Todo
  return false;
}

void
os_print_to_debugger(char const *msg) {
  // @Todo
  (void)msg;
}

void
os_put_breakpoint_here() {
  // @Todo
}

/**
 * File I/O
*/
[[nodiscard]] Buffer
os_read_entire_file_or_panic(char const *path);

void
os_write_entire_file_or_panic(Buffer content, char const *path);

void
os_append_to_file_or_panic(Buffer content, char const *path);

void
os_move_file_or_panic(char const *src, char const *dst);

// pathf?

/**
 * Window
*/
// os_open_window...
// os_get_window_size...

/**
 * Time
*/
struct Time_of_Day {
  s32 day, month, year;
  s32 hours, minutes, seconds;
};

void
os_start_app_timer();

[[nodiscard]] f32
os_get_app_uptime();

// hh:mm:ss, count=const=8.
[[nodiscard]] String
os_get_app_uptime_as_string();

[[nodiscard]] Time_of_Day
os_get_time_of_day();
} // namespace rt