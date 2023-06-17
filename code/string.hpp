namespace rt {
struct String_Builder final {
  char *data = (char*)alloc_temp(128);
  s64 size = 0;
  s64 reserved = 128;
};


template <typename ...TArgs>
void 
appendf(String_Builder &sb, char const *fmt, TArgs ...args);

void
append(String_Builder &sb, String const &string);

[[nodiscard]] String
to_temp_string(String_Builder &sb);

[[nodiscard]] String
to_perm_string(String_Builder &sb);

void
resize_if_needed(String_Builder &sb, s64 size);

template <typename ...TArgs>
[[nodiscard]] String
tprint(char const *fmt, TArgs ...args);

[[nodiscard]] char*
as_cstr(String string);
} // namespace rt