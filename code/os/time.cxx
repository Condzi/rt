namespace rt {
struct alignas(8) Win32_Timer {
  f32             frequency;
  ::LARGE_INTEGER start;
} static gWin32_Timer;

// @Note: some people reported that indeed QueryPerformanceX may return 0
//        if passed params are not aligned to 8 or 4 bytes. I couldn't
//        reproduce it, but I enforce this just in case.
//
//                                                        - kkubacki, 3rd June 2023
static_assert(alignof(Win32_Timer) == 8);

void
os_start_app_timer() {
  ::QueryPerformanceCounter(&gWin32_Timer.start);

  ::LARGE_INTEGER alignas(8) freq;
  ::QueryPerformanceFrequency(&freq);

  check_(freq.QuadPart < FLT_MAX);
  gWin32_Timer.frequency = (f32)freq.QuadPart;
}

[[nodiscard]] f32
os_get_app_uptime() {
  ::LARGE_INTEGER alignas(8) now;
  ::QueryPerformanceCounter(&now);

  u64 const dt = (u64)(now.QuadPart - gWin32_Timer.start.QuadPart);

  return (f32)dt / gWin32_Timer.frequency;
}

// hh:mm:ss, count=const=8.
[[nodiscard]] String
os_get_app_uptime_as_string() {
  s32 seconds = (s32)os_get_app_uptime();
  s32 minutes = seconds / 60;
  s32 hours   = minutes / 60;

  minutes %= 60;
  seconds %= 60;

  String_Builder sb;
  {
    if (hours < 10) appendf(sb, "%s", "0");
    appendf(sb, "%d:", hours);

    if (minutes < 10) appendf(sb, "%s", "0");
    appendf(sb, "%d:", minutes);

    if (seconds < 10) appendf(sb, "%s", "0");
    appendf(sb, "%d", seconds);
  }

  String result = to_temp_string(sb);
  return result;
}

[[nodiscard]] Time_of_Day
os_get_time_of_day() {
  ::SYSTEMTIME st;
  ::GetSystemTime(&st);

  return {.day     = st.wDay,
          .month   = st.wMonth,
          .year    = st.wYear,
          .hours   = st.wHour,
          .minutes = st.wMinute,
          .seconds = st.wSecond};
}
} // namespace rt