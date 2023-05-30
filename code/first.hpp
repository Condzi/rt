::FILE *g_LogFile;

#define logf(...)                      \
  do {                                 \
    fprintf(g_LogFile, __VA_ARGS__);   \
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
