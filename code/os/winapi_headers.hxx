// WinApi headers. DX11 renderer uses this file too.

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <debugapi.h> // __debugbreak, OutputDebugString
// MiniDump {
  #include <Dbghelp.h> 
  #pragma comment(lib, "Dbghelp.lib")
// } MiniDump

// near/far pointers O.O!
#undef near
#undef far