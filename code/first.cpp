#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cfloat>

#include "first.hpp"

#include "config.hpp"
#include "memory.hpp"
#include "string.hpp"
#include "os/os.hpp"

// WinApi Headers {
  #define NOMINMAX
  #define WIN32_LEAN_AND_MEAN
  #include <Windows.h>
  #include <hidusage.h>
  // MiniDump {
    #include <Dbghelp.h> 
    #pragma comment(lib, "Dbghelp.lib")
  // } MiniDump
// } WinApi Headers

#include "memory.cpp"
#include "string.cpp"
#include "os/os.cpp"


using namespace rt;

template <typename ...TArgs> 
[[noreturn]] void
errf(char const *fmt, TArgs ...args) {
  u32 const last_error = os_get_last_error();
  String const last_error_str = os_error_to_string(last_error);

  logf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  logf("        Fatal error!\n    ");
  logf(fmt, args...);
  logf("\n\nLast OS error:\n        %.*s\n",
       (s32)last_error_str.count, last_error_str.data);
  logf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

  ::fflush(stdout);
  ::fflush(gLog_File);
  ::exit(1);
}

int 
main(void) {
  gLog_File = fopen(RT_LOG_FILE, "w");

  logf("Logger initialized!\n");

  if (!init_memory()) {
    errf("init_memory");
  }

  String_Builder sb;

  appendf(sb, "Hello, %s!\n", "World");
  String str = to_string(sb);

  logf("Message: %s", as_cstr(str));

  check_(sb.data != NULL);
  check_(sb.size == 0);
  check_(sb.reserved == 128);

  dbg_check_(false);

  logf("Goodbye :)\n");
  fflush(gLog_File);
  
  return 0;
}