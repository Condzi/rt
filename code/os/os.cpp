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
  ::DWORD constexpr LANG    = 0;
  ::DWORD const     msg_len = ::FormatMessageA(
                                  FLAGS, 
                                  NULL, 
                                  (::DWORD)error_code, 
                                  LANG, 
                                  buff, 
                                  BUFF_SIZE, 
                                  NULL
                                  );

  String result;
  if (msg_len == 0) {
    result = tprint("(FormatMessageA failed with 0x%08x while trying to parse 0x%.8x)"
                    , os_get_last_error(), error_code);
  } else {
    result = tprint("0x%08x: %.*s", error_code, msg_len, buff);
  }

  return result;
}

[[nodiscard]] u32
os_get_last_error() {
  return (u32)::GetLastError();
}

[[nodiscard]] char const*
get_win32_exception_description(::UINT ex_code) {
  switch(ex_code) {
    case EXCEPTION_ACCESS_VIOLATION:         return "EXCEPTION_ACCESS_VIOLATION: The thread tried to read from or write to a virtual address for which it does not have the appropriate access.";
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED: The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking.";
    case EXCEPTION_BREAKPOINT:               return "EXCEPTION_BREAKPOINT: A breakpoint was encountered.";
    case EXCEPTION_DATATYPE_MISALIGNMENT:    return "EXCEPTION_DATATYPE_MISALIGNMENT: The thread tried to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on.";
    case EXCEPTION_FLT_DENORMAL_OPERAND:     return "EXCEPTION_FLT_DENORMAL_OPERAND: One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value.";
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "EXCEPTION_FLT_DIVIDE_BY_ZERO: The thread tried to divide a floating-point value by a floating-point divisor of zero.";
    case EXCEPTION_FLT_INEXACT_RESULT:       return "EXCEPTION_FLT_INEXACT_RESULT: The result of a floating-point operation cannot be represented exactly as a decimal fraction.";
    case EXCEPTION_FLT_INVALID_OPERATION:    return "EXCEPTION_FLT_INVALID_OPERATION: This exception represents any floating-point exception not included in this list.";
    case EXCEPTION_FLT_OVERFLOW:             return "EXCEPTION_FLT_OVERFLOW: The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.";
    case EXCEPTION_FLT_STACK_CHECK:          return "EXCEPTION_FLT_STACK_CHECK: The stack overflowed or underflowed as the result of a floating-point operation.";
    case EXCEPTION_FLT_UNDERFLOW:            return "EXCEPTION_FLT_UNDERFLOW: The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.";
    case EXCEPTION_ILLEGAL_INSTRUCTION:      return "EXCEPTION_ILLEGAL_INSTRUCTION: The thread tried to execute an invalid instruction.";
    case EXCEPTION_IN_PAGE_ERROR:            return "EXCEPTION_IN_PAGE_ERROR: The thread tried to access a page that was not present, and the system was unable to load the page. For example, this exception might occur if a network connection is lost while running a program over the network.";
    case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "EXCEPTION_INT_DIVIDE_BY_ZERO: The thread tried to divide an integer value by an integer divisor of zero.";
    case EXCEPTION_INT_OVERFLOW:             return "EXCEPTION_INT_OVERFLOW: The result of an integer operation caused a carry out of the most significant bit of the result.";
    case EXCEPTION_INVALID_DISPOSITION:      return "EXCEPTION_INVALID_DISPOSITION: An exception handler returned an invalid disposition to the exception dispatcher. Programmers using a high-level language such as C should never encounter this exception.";
    case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION: The thread tried to continue execution after a noncontinuable exception occurred.";
    case EXCEPTION_PRIV_INSTRUCTION:         return "EXCEPTION_PRIV_INSTRUCTION: The thread tried to execute an instruction whose operation is not allowed in the current machine mode.";
    case EXCEPTION_SINGLE_STEP:              return "EXCEPTION_SINGLE_STEP: A trace trap or other single-instruction mechanism signaled that one instruction has been executed.";
    case EXCEPTION_STACK_OVERFLOW:           return "EXCEPTION_STACK_OVERFLOW: The thread used up its stack.";
    default:                                 return "Unknown exception.";
  }
}

[[nodiscard]] bool
is_exception_important(::UINT ex_code) {
  /* @Note: some known exceptions we should not handle:
	 *	Code      |		Info
	 * 0x406D1388 | Microsoft VS Exception
	 * 0x4001000A | Renderdoc
	*/
    
	switch (ex_code) {
		case EXCEPTION_ACCESS_VIOLATION:
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		// case EXCEPTION_BREAKPOINT:
		case EXCEPTION_DATATYPE_MISALIGNMENT:
		case EXCEPTION_FLT_DENORMAL_OPERAND:
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		case EXCEPTION_FLT_INEXACT_RESULT:
		case EXCEPTION_FLT_INVALID_OPERATION:
		case EXCEPTION_FLT_OVERFLOW:
		case EXCEPTION_FLT_STACK_CHECK:
		case EXCEPTION_FLT_UNDERFLOW:
		case EXCEPTION_ILLEGAL_INSTRUCTION:
		case EXCEPTION_IN_PAGE_ERROR:
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
		case EXCEPTION_INT_OVERFLOW:
		case EXCEPTION_INVALID_DISPOSITION:
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		case EXCEPTION_PRIV_INSTRUCTION:
		case EXCEPTION_SINGLE_STEP:
		case EXCEPTION_STACK_OVERFLOW: {
			return true;
		}
		default: return false;
	}
}

[[nodiscard]] bool
write_minidump(::EXCEPTION_POINTERS *ex_ptrs) {
  // @todo: use pathf!
  char dump_path[] = "dump.dmp";
  ::HANDLE file;
	file = ::CreateFileA(dump_path, 
                       GENERIC_WRITE,
                       FILE_SHARE_READ, 
                       NULL,
                       CREATE_ALWAYS, 
                       FILE_ATTRIBUTE_NORMAL, 
                       NULL
                       );

	if (file == INVALID_HANDLE_VALUE) {
		logf("Failed to create file for MiniDump! (\"%s\")\n", dump_path);
    return false;
	} 
  
  logf("Writing MiniDump...\n");

  ::MINIDUMP_EXCEPTION_INFORMATION MiniDumpInfo {
    .ThreadId = ::GetCurrentThreadId(),
    .ExceptionPointers = ex_ptrs,
    .ClientPointers = false
  };

  ::BOOL const success = ::MiniDumpWriteDump(::GetCurrentProcess(), 
                                             ::GetCurrentProcessId(),
                                             file, 
                                             MINIDUMP_TYPE::MiniDumpNormal, 
                                             &MiniDumpInfo, 
                                             NULL, 
                                             NULL
                                             );
                                        
	::CloseHandle(file);

  if (success) {
    logf("MiniDump succesfully saved to \"%s\"!\n", dump_path);
  } else {
    // @Robustness: check if MiniDumpWriteDump set the GetLastError value.
    logf("Failed to write MiniDump!\n");
  }

  return (bool)success;
}


extern "C" ::LONG 
vectored_exception_handler_proc(::EXCEPTION_POINTERS *ex_ptrs) {
  ::DWORD const ex_code = ex_ptrs->ExceptionRecord[0].ExceptionCode;
  if (!is_exception_important(ex_code)) {
    return EXCEPTION_CONTINUE_SEARCH;
  }

  logf("================================\n");
	logf("\t\t Exception caught!\n");
	logf(" 0x%08x\n", ex_code);
	logf(" %s\n", get_win32_exception_description(ex_code));
	logf("================================\n");

  bool const dump_written = write_minidump(ex_ptrs);

  // Prepare the final message.

  String_Builder sb;
  appendf(sb, "Engine has encountered a problem.\r\n\r\n");
	appendf(sb, "You can find the log file in:\r\n\t%s.\r\n\r\n", "todo"); // @Todo: use pathf!!

	if (dump_written) {
		appendf(sb, "Minidump written to:\r\n\t%s.", "todo"); // @Todo: use pathf!
	} else {
		appendf(sb, "MiniDump not created. See logs for the reason.");
	}

	char const *user_msg = as_cstr(to_string(sb));
	// ShutdownLog();
	// NOTE(konrad): for simplicity, we use ASCII version of MessageBox.
	::MessageBoxA(NULL, user_msg, " Crash :(", MB_OK | MB_ICONERROR);
	::ExitProcess(2);
}

void 
os_register_vectored_exception_handler() {
  if (os_is_debugger_present()) {
    logf("VEH: disabled\n");
    return;
  }

  ::AddVectoredExceptionHandler(1, vectored_exception_handler_proc);

  logf("VEH: enabled\n");
}

/**
 * Debugging
*/

[[nodiscard]] bool
os_is_debugger_present() {
  return ::IsDebuggerPresent();
}

void
os_print_to_debugger(char const *msg) {
  // @Unicode
  ::OutputDebugStringA(msg);
}

void
os_put_breakpoint_here() {
  ::__debugbreak();
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
    errf("Failed to open file for reading '%s'.", path);
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

  void *view = ::MapViewOfFile(
                  mapping, 
                  FILE_MAP_READ, 
                  0, 
                  0, 
                  0
                  );
                  
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

void
os_write_entire_file_or_panic(Buffer content, char const *path) {
  check_(path);
  check_(content.count >= 0);
  check_(content.bytes != NULL);

  // @Unicode: handle UTF-8 paths
  ::HANDLE file = ::CreateFileA(
                      (LPCSTR)path, 
                      GENERIC_WRITE, 
                      FILE_SHARE_READ,
                      0,
                      CREATE_ALWAYS,
                      FILE_ATTRIBUTE_NORMAL,
                      NULL
                      );

  if (file == INVALID_HANDLE_VALUE) {
    errf("Failed to open file for writing: '%s'", path);
  }
  
  ::DWORD const bytes_to_write = (::DWORD)content.count;
  ::DWORD bytes_written = 0;
  ::BOOL const success = ::WriteFile(
                           file, 
                           content.bytes, 
                           bytes_to_write, 
                           &bytes_written, 
                           NULL
                           );

  if (!success) {
    errf("Failed to write to file '%s'", path);
  }

  if (bytes_written != bytes_to_write) {
    errf("Partial write to '%s' (%zu/%zu)", path, bytes_written, bytes_to_write);
  }
  
  ::CloseHandle(file);
}

void
os_append_to_file_or_panic(Buffer content, char const *path) {
  check_(path);
  check_(content.count >= 0);
  check_(content.bytes != NULL);

  // @Unicode: handle UTF-8 paths
  ::HANDLE file = ::CreateFileA(
                      (LPCSTR)path, 
                      FILE_APPEND_DATA, // @Note: the only part that is different from write_entire_file 
                      FILE_SHARE_READ,
                      0,
                      CREATE_ALWAYS,
                      FILE_ATTRIBUTE_NORMAL,
                      NULL
                      );

  if (file == INVALID_HANDLE_VALUE) {
    errf("Failed to open file for appending: '%s'", path);
  }
  
  ::DWORD const bytes_to_write = (::DWORD)content.count;
  ::DWORD bytes_written = 0;
  ::BOOL const success = ::WriteFile(
                           file, 
                           content.bytes, 
                           bytes_to_write, 
                           &bytes_written, 
                           NULL
                           );

  if (!success) {
    errf("Failed to append to file '%s'", path);
  }

  if (bytes_written != bytes_to_write) {
    errf("Partial write (append) to '%s' (%zu/%zu)", path, bytes_written, bytes_to_write);
  }
  
  ::CloseHandle(file);
}

void
os_move_file_or_panic(char const *src, char const *dst) {
  check_(src);
  check_(dst);

  ::BOOL const success = ::MoveFileA(src, dst);

  if (!success) {
    errf("Failed to move file from '%s' to '%s'", src, dst);
  }
}


struct alignas(8) Win32_Timer {
  f32 frequency;
  ::LARGE_INTEGER start;
} static gWin32_Timer;

// NOTE(konrad): some people reported that indeed QueryPerformanceX can return 0, 
//               if passed params are not aligned to 8 or 4 bytes. I couldn't 
//               reproduce it, but I left thid just in case.
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
  s32 seconds = (s32)os_get_app_uptime();
	s32 minutes = seconds/60;
	s32 hours   = minutes/60;

	minutes %= 60;
	seconds %= 60;

	String_Builder sb;
  {
    if (hours < 10)   appendf(sb, "%s", "0");
    appendf(sb, "%d:", hours);

    if (minutes < 10) appendf(sb, "%s", "0");
    appendf(sb, "%d:", minutes);

    if (seconds < 10) appendf(sb, "%s", "0");
    appendf(sb, "%d", seconds);
  }

	String result = to_string(sb);
  return result;
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