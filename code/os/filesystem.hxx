/**
 * File I/O
*/
namespace rt {
[[nodiscard]] Buffer
os_read_entire_file_or_panic(char const *path);

void
os_write_entire_file_or_panic(Buffer content, char const *path);

void
os_append_to_file_or_panic(Buffer content, char const *path);

void
os_move_file_or_panic(char const *src, char const *dst);

// pathf?
} // namespace rt