#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cfloat>
#include <cmath>

#include "first.hpp"

#include "config.hpp"
#include "base/base.hpp"
#include "math/math.hpp"
#include "os/os.hpp"
#include "window/window.hpp"
#include "gfx/gfx.hpp"

#include "base/base.cpp"
#include "math/math.cpp"
#include "os/os.cpp"
#include "window/window.cpp"
#include "gfx/gfx.cpp"


using namespace rt;

template <typename ...TArgs> 
[[noreturn]] void
errf(char const *fmt, TArgs ...args) {
  u32    const last_error     = os_get_last_error();
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

  Vec2 vec2{1,1};
  vec2 = Vec2{2,2};
  Vec4 vec{.xy = vec2};

  logf("Logger initialized!\n");

  if (!init_memory()) {
    errf("init_memory");
  }

  os_start_app_timer();
  os_init_filesystem();
  window_create_or_panic();
  gfx_init_or_panic();

  String_Builder sb;

  appendf(sb, "Hello, %s!\n", "World");
  String str = to_temp_string(sb);

  logf("Message: %s", as_cstr(str));

  check_(sb.data != NULL);
  check_(sb.size == 0);
  check_(sb.reserved == 128);

  dbg_check_(false);

  while(!window_is_closed()) {
    win32_message_loop();
    gfx_render();
  }
  
  logf("Goodbye :)\n");
  fflush(gLog_File);

  
  return 0;
}