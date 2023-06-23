#define RT_LOG_FILE "log.txt"

#define TEMP_MEM_SIZE RT_MEGABYTES(8)

namespace rt {
char const static      *WIN_NAME  = "Raytracing Engine";
char const static      *WIN_CLASS = "rt_engine";
s64  constexpr static   WIN_WIDTH = 1366;
s64  constexpr static  WIN_HEIGHT = 768;
bool constexpr static       VSYNC = true;
} // namespace rt