#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cfloat>
#include <cmath>
#include <stdarg.h> // logf

#include "first.hpp"

#include "config.hpp"
#include "base/base.hpp"
#include "math/math.hpp"
#include "os/os.hpp"
#include "window/window.hpp"
#include "gfx/gfx.hpp"
#include "imgui/imgui.hpp"
#include "parsers/parsers.hpp"
#include "cpu_rt/cpu_rt.hpp"

#include "base/base.cpp"
#include "math/math.cpp"
#include "os/os.cpp"
#include "window/window.cpp"
#include "gfx/gfx.cpp"
#include "imgui/imgui.cpp"
#include "parsers/parsers.cpp"
#include "cpu_rt/cpu_rt.cpp"

using namespace rt;

template <typename... TArgs>
[[noreturn]] void
errf(char const *fmt, TArgs... args) {
  u32 const    last_error     = os_get_last_error();
  String const last_error_str = os_error_to_string(last_error);

  logf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  logf("        Fatal error!\n    ");
  logf(fmt, args...);
  logf("\n\nLast OS error:\n        %.*s\n",
       (s32)last_error_str.count,
       last_error_str.data);
  logf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

  ::fflush(stdout);
  ::fflush(gLog_File);
  ::exit(1);
}

int
main(void) {
  gLog_File = fopen(RT_LOG_FILE, "w");

  Vec2 vec2 {1, 1};
  vec2 = Vec2 {2, 2};
  Vec4 vec {.xy = vec2};

  logf("Logger initialized!\n");

  if (!init_memory()) {
    errf("init_memory");
  }

  os_start_app_timer();
  os_init_filesystem();
  window_create_or_panic();
  gfx_init_or_panic();
  dear_imgui_init();

  String_Builder sb;

  appendf(sb, "Hello, %s!\n", "World");
  String str = to_temp_string(sb);

  logf("Message: %s", as_cstr(str));

  check_(sb.data != NULL);
  check_(sb.size == 0);
  check_(sb.reserved == 128);

  dbg_check_(false);

  Rt_Output rt_out = do_raytraycing();

  // write_png_or_panic("hello_raytraycing.png", rt_out.rgba_data, rt_out.image_size);
  ImTextureID const rt_out_as_texture =
      dear_imgui_create_texture_from_rt_output(rt_out);

  while (!window_is_closed()) {
    win32_message_loop();

    gfx_im_rect({.x = 650, .y = 400}, {.width = 50, .height = 100}, COLOR_RED);
    gfx_im_rect({.x = 600, .y = 300}, {.width = 50, .height = 100}, COLOR_GREEN);
    gfx_im_rect({.x = 700, .y = 300}, {.width = 50, .height = 100}, COLOR_BLUE);

    dear_imgui_update();

    ImGui::Begin("CPU Raytracing");
    ImGui::Image(rt_out_as_texture,
                 ImVec2(rt_out.image_size.width*2, rt_out.image_size.height*2));
    ImGui::End();

    gfx_render();
  }

  logf("Goodbye :)\n");
  fflush(gLog_File);

  return 0;
}
