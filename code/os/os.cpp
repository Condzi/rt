namespace rt {
[[nodiscard]] String
os_error_to_string(u32 error_code) {
  // @Note: FormatMessage, unlike other sprintf-like functions, doesn't return 
  //        buffer size needed to encode the message when we pass NULL. Therefore, we
  //        allocate large enough buffer and hope it is enough. 
  //        According to some StackOverflow post, .NET code uses 
  //        4096 * sizeof(wchar_t) = 8k buffer. Since this is
  //        what Microsoft does, we also use this size and hope for the best.
  //        
  //                                                        - kkubacki, 2nd June 2023
  //
  s64 constexpr static BUFF_SIZE = RT_KILOBYTES(8);
  char *buff = (char*)alloc_temp(BUFF_SIZE);

  ::DWORD constexpr FLAGS = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
  // @Unicode: We can't force the message to be in English because non-english OS
  //           versions may not have this locale. We will just print the message with
  //           raw bytes in the client language and hope it's readable.
  //        
  //                                                        - kkubacki, 2nd June 2023
  //
  ::DWORD constexpr LANG  = 0;
  ::DWORD const     msg_len = ::FormatMessageA(FLAGS, NULL, (::DWORD)error_code, LANG, 
                                               buff, BUFF_SIZE, NULL);

  String result;
  if (msg_len == 0) {
    result = tprint("(FormatMessageA failed with 0x%.8x while trying to parse 0x%.8x)"
                    , os_get_last_error(), error_code);
  } else {
    result = tprint("0x%.8x: %.*s", error_code, msg_len, buff);
  }

  return result;
}

[[nodiscard]] u32
os_get_last_error() {
  return (u32)::GetLastError();
}


[[nodiscard]] Buffer
os_read_entire_file_or_panic(char const *path) {
  // @Note: we don't care about leaking resources on error, because system will
  //        close all handles for us.
  check_(path);

  // @Unicode: handle UTF-8 paths
  ::HANDLE file = ::CreateFileA(
                      (LPCSTR)path, 
                      GENERIC_READ, 
                      NULL,
                      0,
                      OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL,
                      NULL
                      );

  if (file == INVALID_HANDLE_VALUE) {
    errf("Failed to open file from '%s'.", path);
  }

  ::HANDLE mapping = ::CreateFileMappingA(
                        file,
                        NULL,
                        PAGE_READONLY,
                        0,
                        0,
                        NULL
                        );

  if (mapping == INVALID_HANDLE_VALUE) {
    errf("Failed to create mapping of file '%s'", path);
  }

  void *view = ::MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
  if (view == NULL) {
    errf("Failed to map file '%s'", path);
  }
	
  ::LARGE_INTEGER file_sz_raw;
	::GetFileSizeEx(file, &file_sz_raw);
	s64 const file_sz = file_sz_raw.QuadPart;

	void *file_mem = alloc_temp(file_sz);
	::memcpy(file_mem, view, (u64)file_sz);

  ::UnmapViewOfFile(view);
  ::CloseHandle(mapping);
  ::CloseHandle(file);

	return {.count = file_sz, .bytes = (u8*)file_mem};
}


struct alignas(8) Win32_Timer {
  f32 frequency;
  ::LARGE_INTEGER start;
} static gWin32_Timer;

// NOTE(konrad): some people reported that indeed QueryPerformanceX can return 0, 
//               if passed params are not aligned to 8 or 4 bytes. I couldn't 
//               reproduce it, but I left these just in case.
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
	
	return (f32)dt/gWin32_Timer.frequency;
}

// hh:mm:ss, count=const=8.
[[nodiscard]] String
os_get_app_uptime_as_string() {
  // @Todo: implement with String_Builder
  return {};
}

[[nodiscard]] Time_of_Day
os_get_time_of_day() {
  ::SYSTEMTIME st;
	::GetSystemTime(&st);

	return {
		.day     = st.wDay,
		.month   = st.wMonth,
		.year    = st.wYear,
		.hours   = st.wHour,
		.minutes = st.wMinute,
		.seconds = st.wSecond
	};
}
} // namespace rt