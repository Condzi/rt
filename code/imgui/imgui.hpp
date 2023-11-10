#define IMGUI_DEFINE_MATH_OPERATORS
#include "3rdparty/imgui/imgui.h"

namespace rt {
void
dear_imgui_init();

void
dear_imgui_update();

void
dear_imgui_draw();

[[nodiscard]] ImTextureID
dear_imgui_create_texture_from_rt_output(struct CPU_RT_Output const &rt_out);

void
dear_imgui_delete_texture(ImTextureID texture);

// @Note: window proc handler is defined in imgui.cpp, but forward declared in
// wnd_proc.cxx
} // namespace rt
