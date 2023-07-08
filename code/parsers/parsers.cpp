#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "3rdparty/stb/stb_image_write.h"

namespace rt {
void
write_png_or_panic(char const *path, Buffer data, Vec2 size) {
  s32 constexpr static NUM_OF_CHANNELS = 3; // RGB
  s32 const width                      = (s32)size.width;
  s32 const height                     = (s32)size.height;
  s32 const stride                     = width * NUM_OF_CHANNELS;

  check_(path);
  check_(data.count == width * height * NUM_OF_CHANNELS);
  check_(data.bytes);

  int status =
      stbi_write_png(path, width, height, NUM_OF_CHANNELS, data.bytes, stride);

  if (status == 0) {
    errf("Failed to write the PNG to %s.", path);
  } else {
    logf("Saved %dx%d PNG to %s.\n", width, height, path);
  }
}
} // namespace rt
