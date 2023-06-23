
namespace rt {
namespace impl {
/**
 * Functions related to VEH procedure 
*/
[[nodiscard]] char const*
get_win32_exception_description(::UINT ex_code);

[[nodiscard]] bool
is_exception_important(::UINT ex_code);

[[nodiscard]] bool
write_minidump(::EXCEPTION_POINTERS *ex_ptrs);

extern "C" ::LONG 
vectored_exception_handler_proc(::EXCEPTION_POINTERS *ex_ptrs);
} // namespace impl

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

void 
os_register_vectored_exception_handler() {
  if (os_is_debugger_present()) {
    logf("VEH: disabled\n");
    return;
  }

  ::AddVectoredExceptionHandler(1, impl::vectored_exception_handler_proc);

  logf("VEH: enabled\n");
}

namespace impl {
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
  // @Copypasta
  String path = pathf("%l\\minidump.dmp");
  ::HANDLE file;
	file = ::CreateFileA(as_cstr(path), 
                       GENERIC_WRITE,
                       FILE_SHARE_READ, 
                       NULL,
                       CREATE_ALWAYS, 
                       FILE_ATTRIBUTE_NORMAL, 
                       NULL
                       );

	if (file == INVALID_HANDLE_VALUE) {
		logf("Failed to create file for MiniDump! (\"%.*s\")\n",
         (int)path.count, path.data);
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
    logf("MiniDump succesfully saved to \"%.*s\"!\n", (int)path.count, path.data);
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
	appendf(sb, "You can find the log file in:\r\n\t%.*s.\r\n\r\n", 
          (int)gPath_Cache.logs.count, gPath_Cache.logs.data);

	if (dump_written) {
    // @Copypasta
    String path = pathf("%l\\minidump.dmp");
		appendf(sb, "Minidump written to:\r\n\t%.*s.", (int)path.count, path.data);
	} else {
		appendf(sb, "MiniDump not created. See logs for the reason.");
	}

	char const *user_msg = as_cstr(to_temp_string(sb));
	// ShutdownLog();
	// NOTE(konrad): for simplicity, we use ASCII version of MessageBox.
	::MessageBoxA(NULL, user_msg, " Crash :(", MB_OK | MB_ICONERROR);
	::ExitProcess(2);
}
} // namespace impl
} // namespace rt