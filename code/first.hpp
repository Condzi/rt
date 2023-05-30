#ifdef _WIN32
  #ifndef _WIN64
    #error 32-bit Windows is not supported.
  #endif // _WIN64
#else
  #error Platform not supported.
#endif // _WIN32

#ifndef _MSC_VER
  #error The only supported compiler is MSVC. 
#endif // _MSVC

namespace rt {
::FILE *gLog_File;

using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using s8  = signed char;
using s16 = signed short;
using s32 = signed int;
using s64 = signed long long;

using f32 = float;
using f64 = double;

struct Buffer {
  s64 count;
  u8 *bytes;
};

struct String {
  s64 count;
  char *data;
};
} // namespace rt

#define logf(...)                      \
  do {                                 \
    fprintf(gLog_File, __VA_ARGS__);   \
    printf(__VA_ARGS__);               \
  } while (false)
  

#define check_(x)                                     \
  do {                                                \
    if (!(x)) {                                       \
      logf("!!! Check failed!\n");                    \
      logf("!!! \t" #x "\n");                         \
      logf("!!! %s:%d\n", __FILE__, (int)__LINE__);   \
    }                                                 \
  } while (false)

#ifdef NDEBUG
  #define dbg_check_(x) __noop
#else
  #define dbg_check_(x) \
    do {                                                \
      if (!(x)) {                                       \
        logf("!!! Debug check failed!\n");              \
        logf("!!! \t" #x "\n");                         \
        logf("!!! %s:%d\n", __FILE__, (int)__LINE__);   \
      }                                                 \
    } while (false)
#endif
