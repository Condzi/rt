namespace rt {

template <typename... TArgs>
void
appendf(String_Builder &sb, char const *fmt, TArgs... args) {
  check_(sb.data != NULL);
  check_(sb.reserved > 0);
  check_(fmt);

  if constexpr (sizeof...(TArgs) == 0) {
    String const string {
        .count = (s64)::strlen(fmt),
        .data  = as_cstr(tprint("fmt")) // @Hack: can't assign const char* to char*
    };

    append(sb, string);
  } else {
    s64 const status = ::snprintf(NULL, 0, fmt, args...);
    check_(status >= 0);

    s64 const bufsz = 1 + status;
    resize_if_needed(sb, bufsz);

    ::snprintf(sb.data + sb.size, bufsz, fmt, args...);

    sb.size += (bufsz - 1);
  }
}

void
append(String_Builder &sb, String const &string) {
  check_(sb.data != NULL);
  check_(string.data != NULL);

  resize_if_needed(sb, string.count);

  ::memcpy(sb.data + sb.size, string.data, string.count);
  sb.size += string.count;
}

[[nodiscard]] String
to_temp_string(String_Builder &sb) {
  check_(sb.data != NULL);

  char *buffer = (char *)alloc_temp(sb.size);
  ::memcpy(buffer, sb.data, sb.size);

  return {.count = sb.size, .data = buffer};
}

[[nodiscard]] String
to_perm_string(String_Builder &sb) {
  check_(sb.data != NULL);

  char *buffer = perm<char>(sb.size);
  ::memcpy(buffer, sb.data, sb.size);

  return {.count = sb.size, .data = buffer};
}

void
resize_if_needed(String_Builder &sb, s64 size) {
  check_(sb.data != NULL);

  if (sb.size + size >= sb.reserved) {
    s64 const needed_size = sb.size + size;
    s64 const quotient    = needed_size / sb.reserved;
    s64 const new_size    = ((quotient + 1) * sb.reserved);

    char *new_buffer = (char *)alloc_temp(new_size);
    ::memcpy(new_buffer, sb.data, sb.size);

    sb.data     = new_buffer;
    sb.reserved = new_size;
  }
}

template <typename... TArgs>
[[nodiscard]] String
tprint(char const *fmt, TArgs... args) {
  s64 const status = ::snprintf(NULL, 0, fmt, args...); // @crt
  check_(status >= 0);
  s64 const bufsz = 1 + status;
  char     *buff  = (char *)alloc_temp(bufsz);

  ::snprintf(buff, bufsz, fmt, args...); // @crt

  return {.count = bufsz - 1, .data = buff};
}

[[nodiscard]] char *
as_cstr(String string) {
  char *buff = (char *)alloc_temp(string.count + 1);
  ::memcpy(buff, string.data, string.count); // @crt
  buff[string.count] = 0;

  return buff;
}
} // namespace rt
