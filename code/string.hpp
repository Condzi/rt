namespace rt {
struct String_Builder final {
  char *data = (char*)alloc_temp(128);
  s64 size = 0;
  s64 reserved = 128;
};



template <typename ...TArgs>
[[nodiscard]] String
tprint(char const *fmt, TArgs ...args);

[[nodiscard]] char*
as_cstr(String string);
} // namespace rt