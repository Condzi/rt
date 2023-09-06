namespace rt {
// Data is RBG, left-to-right, top-to-bottom.
void
write_png_or_panic(char const *path, Buffer data, Vec2 size);
} // namespace rt
