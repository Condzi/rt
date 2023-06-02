namespace rt {
template <typename ...TArgs>
[[nodiscard]] String
tprint(char const *fmt, TArgs ...args);

[[nodiscard]] char*
as_cstr(String string);
} // namespace rt