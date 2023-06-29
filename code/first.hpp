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

void logf(const char *format, ...) {
  va_list args1;
  va_list args2;

  va_start(args1, format);
  va_copy(args2, args1);

  vfprintf(gLog_File, format, args1);
  vprintf(format, args2);

  va_end(args1);
  va_end(args2);
}
} // namespace rt


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

#define RT_KILOBYTES(x) (x*1024)
#define RT_MEGABYTES(x) (RT_KILOBYTES(x)*1024)

#define mem_comp_ ::memcmp
#define mem_copy_ ::memcpy

namespace rt {
template <typename T>
void rt_swap(T &a, T &b) {
	size_t constexpr static size = sizeof(T);
	u8 buf[size];
  
	mem_copy_(buf, &a,  size);
	mem_copy_(&a,  &b,  size);
	mem_copy_(&b,  buf, size);
}
}

template <typename ...TArgs> 
[[noreturn]] void
errf(char const *fmt, TArgs ...args);
