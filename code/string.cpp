namespace rt {
template <typename ...TArgs>
[[nodiscard]] String
tprint(char const *fmt, TArgs ...args) {
  s64 const status = ::snprintf(NULL, 0, fmt, args...); // @crt
  check_(status >= 0);
  s64 const bufsz = 1 + status;
  char *buff = (char*)alloc_temp(bufsz);

  ::snprintf(buff, bufsz, fmt, args...); // @crt

  return {.count = bufsz - 1, .data = buff};
}

[[nodiscard]] char*
as_cstr(String string) {
  char *buff = (char*)alloc_temp(string.count + 1);
  ::memcpy(buff, string.data, string.count); // @crt
  buff[string.count] = 0;

  return buff;
}
} // namespace rt