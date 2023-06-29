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

// Set up the path cache used by pathf
void
os_init_filesystem();

/**
 * Creates a normalized path from a format string. Available modifiers:
 * 
 *  %c -- current working directory (W:\pwr/engineering-thesis/rt/run_tree)
 *  %l -- %c/logs
 *  %d -- %c/data
 *  %S -- %d/shaders
 *  %a -- %c/assets
 *  %t -- %a/textures
 *  %m -- %a/models
*/
[[nodiscard]] String
pathf(char const *fmt);
} // namespace rt