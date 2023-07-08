namespace rt {
void
window_create_or_panic();

[[nodiscard]] Vec2
window_get_size();

[[nodiscard]] bool
window_is_open();

[[nodiscard]] bool
window_is_closed();

[[nodiscard]] bool
window_is_minimized();

void
window_close();
} // namespace rt