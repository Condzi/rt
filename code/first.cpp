#include <immintrin.h> // For SSE intrinsics

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cfloat>
#include <cmath>
#include <stdarg.h> // logf

#include <array>
#include <algorithm> // std::sort in bvh
#include <atomic>
#include <thread>
#include <iterator> // std::back_inserter
#include <unordered_map>

#include "first.hpp"

#include "config.hpp"
#include "base/base.hpp"
#include "math/math.hpp"
#include "os/os.hpp"
#include "window/window.hpp"
#include "gfx/gfx.hpp"
#include "imgui/imgui.hpp"
#include "parsers/parsers.hpp"
#include "base_rt/base_rt.hpp"
#include "cpu_rt/cpu_rt.hpp"

#include "base/base.cpp"
#include "math/math.cpp"
#include "os/os.cpp"
#include "window/window.cpp"
#include "gfx/gfx.cpp"
#include "imgui/imgui.cpp"
#include "parsers/parsers.cpp"
#include "base_rt/base_rt.cpp"
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
  World_Type const world_type = WorldType_Book1Final;

  gLog_File = fopen(RT_LOG_FILE, "w");

  logf("Logger initialized!\n");

  if (!init_memory()) {
    errf("init_memory");
  }

  os_start_app_timer();
  os_init_filesystem();
  window_create_or_panic();
  gfx_init_or_panic();
  dear_imgui_init();

  f32 cpu_start_time = os_get_app_uptime();
  f32 cpu_end_time   = 0;

  // Common RT Setup
  //

  f32 const aspect_ratio = 1;
  s32 const image_width  = 512;
  s32 const image_height = (s32)(image_width / aspect_ratio);

  // Camera setup
  //

  Camera_Parameters const cam_Book1Final {.center         = {13, 2, 3},
                                          .look_at        = {0, 0, 0},
                                          .up             = {0, 1, 0},
                                          .vfov           = 20.0f,
                                          .aspect_ratio   = aspect_ratio,
                                          .aperture       = 0.1f,
                                          .focus_distance = 15.0f};
  Camera_Parameters const cam_Quads {.center         = {0, 0, 9},
                                     .look_at        = {0, 0, 0},
                                     .up             = {0, 1, 0},
                                     .vfov           = 80.0f,
                                     .aspect_ratio   = aspect_ratio,
                                     .aperture       = 0.1f,
                                     .focus_distance = 15.0f};

  Camera_Parameters const cam_SimpleLights {.center         = {26, 3, 6},
                                            .look_at        = {0, 2, 0},
                                            .up             = {0, 1, 0},
                                            .vfov           = 20.0f,
                                            .aspect_ratio   = aspect_ratio,
                                            .aperture       = 0.1f,
                                            .focus_distance = 15.0f};

  Camera_Parameters const camera_presets[WorldType__count] = {
      cam_Book1Final, cam_Quads, cam_SimpleLights};

  Camera cam = make_camera(camera_presets[world_type]);

  // World
  World w = create_world(world_type);
  // Generate list of BVH_Input based on object IDs.
  //
  std::vector<BVH_Input> bvh_input;
  bvh_input.reserve(w.num_spheres + w.num_quads);
  for (s32 i = 0; i < w.num_spheres; i++) {
    Object_ID id {.idx = (u32)i, .type = ObjectType_Sphere};
    bvh_input.emplace_back(id, w.spheres[i].aabb);
  }
  for (s32 i = 0; i < w.num_quads; i++) {
    Object_ID id {.idx = (u32)i, .type = ObjectType_Quad};
    bvh_input.emplace_back(id, w.quads[i].aabb);
  }

  std::vector<BVH_Flat> bvh =
      make_BVH(bvh_input.data(), 0, (s32)bvh_input.size(), w.aabb);

  // Common RT Setup end

  GFX_RT_Input gpu_in {
      .im_size = {(f32)image_width, (f32)image_height}, .w = w, .c = cam};
  gfx_rt_init_or_panic(gpu_in);

  CPU_RT_Input cpu_in {
      .im_size = {(f32)image_width, (f32)image_height}, .w = w, .c = cam, .bvh = bvh};
  CPU_RT_Output rt_out = do_ray_tracing(cpu_in);

  f32 const   gpu_start_time = os_get_app_uptime();
  f32         gpu_end_time   = gpu_start_time;
  ImTextureID rt_tex         = gfx_rt_output_as_imgui_texture();
  gfx_rt_start();

  // write_png_or_panic("hello_ray_tracing.png", rt_out.rgba_data,
  // rt_out.image_size);
  while (!window_is_closed()) {
    if (f32_compare(gpu_start_time, gpu_end_time) && gfx_rt_done()) {
      rt_tex       = gfx_rt_output_as_imgui_texture();
      gpu_end_time = os_get_app_uptime();
    }

    win32_message_loop();

    gfx_im_rect({.x = 650, .y = 400}, {.width = 50, .height = 100}, COLOR_RED);
    gfx_im_rect({.x = 600, .y = 300}, {.width = 50, .height = 100}, COLOR_GREEN);
    gfx_im_rect({.x = 700, .y = 300}, {.width = 50, .height = 100}, COLOR_BLUE);

    dear_imgui_update();

    ImGui::Begin("CPU Ray Tracing");
    // @Note: we do it over and over again because RT is ray tracing all the time
    //       In future just add an atomic that counts number of threads finished.
    ImTextureID rt_out_as_texture = dear_imgui_create_texture_from_rt_output(rt_out);
    ImGui::Image(rt_out_as_texture,
                 ImVec2(rt_out.image_size.width, rt_out.image_size.height));
    ImGui::End();

    ImGui::Begin("GPU Ray Tracing");
    if (rt_tex) {
      ImGui::Text("Finished in %g seconds.", gpu_end_time - gpu_start_time);
      ImGui::Image(rt_tex, ImVec2(512, 512));
    } else {
      ImGui::Text("Elapsed: %g seconds.", os_get_app_uptime() - gpu_start_time);
    }
    ImGui::End();

    ImGui::Begin("Threads");
    ImGui::Text("%d threads used", rt_out.num_threads);
    s32 num_finished = 0;
    for (s32 i = 0; i < rt_out.num_threads; i++) {
      ImGui::Text("Thread %d: ", i + 1);
      ImGui::SameLine();
      if (rt_out.thread_flags[i]) {
        ImGui::Text("Finished");
        num_finished++;
      } else {
        ImGui::Text("Running");
      }
    }

    if (num_finished == rt_out.num_threads) {
      if (f32_compare(cpu_end_time, 0)) {
        cpu_end_time = os_get_app_uptime();
      }
      f32 const time = cpu_end_time - cpu_start_time;
      ImGui::Text("Finished in %g seconds.", time);
      ImGui::Text("%g Mrays/s", ((s64)total_ray_count / time) / 1'000'000);
    } else {
      f32 const time = os_get_app_uptime() - cpu_start_time;
      ImGui::Text("Elapsed: %g seconds.", time);
      ImGui::Text("%g Mrays/s", ((s64)total_ray_count / time) / 1'000'000);
    }
    ImGui::End();

    gfx_render();

    clear_temp_mem();

    auto res = (ID3D11ShaderResourceView *)rt_out_as_texture;
    d3d_safe_release_(res);
    }

  logf("Goodbye :)\n");
  fflush(gLog_File);

  return 0;
}
