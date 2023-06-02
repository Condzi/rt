namespace rt {
/**
 * Error handling
*/
[[nodiscard]] String
os_error_to_string(u32 error_code);

[[nodiscard]] u32
os_get_last_error();

/**
 * File I/O
*/
[[nodiscard]] Buffer
os_read_entire_file_or_panic(char const *path);
// os_write_entire_file_or_panic
// os_append_to_file_or_panic
// os_move_file_or_panic
// pathf?

/**
 * Window
*/
// os_open_window...
// os_get_window_size...

/**
 * Time
*/
// os_get_seconds_since_startup
// os_get_date
} // namespace rt