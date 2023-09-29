// WinApi headers. DX11 renderer uses this file too.

#define WIN32_LEAN_AND_MEAN

#define NOGDICAPMASKS // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
// #define NOVIRTUALKEYCODES // VK_*
// #define NOWINMESSAGES     // WM_*, EM_*, LB_*, CB_*
// #define NOWINSTYLES       // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
// #define NOSYSMETRICS      // SM_*
// #define NOMENUS           // MF_*
// #define NOICONS           // IDI_*
// #define NOKEYSTATES       // MK_*
// #define NOSYSCOMMANDS     // SC_*
#define NORASTEROPS // Binary and Tertiary raster ops
// #define NOSHOWWINDOW      // SW_*
#define OEMRESOURCE // OEM Resource values
// #define NOATOM            // Atom Manager routines
// #define NOCLIPBOARD       // Clipboard routines
// #define NOCOLOR           // Screen colors
// #define NOCTLMGR          // Control and Dialog routines
#define NODRAWTEXT // DrawText() and DT_*
// #define NOGDI             // All GDI defines and routines
#define NOKERNEL // All KERNEL defines and routines
// #define NOUSER            // All USER defines and routines
// #define NONLS             // All NLS defines and routines (MultByteToWideChar etc)
// #define NOMB              // MB_* and MessageBox()
#define NOMEMMGR   // GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE // typedef METAFILEPICT
#define NOMINMAX   // Macros min(a,b) and max(a,b)
// #define NOMSG             // typedef MSG and associated routines
#define NOOPENFILE // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
// #define NOSCROLL          // SB_* and scrolling routines
#define NOSERVICE // All Service Controller routines, SERVICE_ equates, etc.
// #define NOSOUND           // Sound driver routines
#define NOTEXTMETRIC     // typedef TEXTMETRIC and associated routines
#define NOWH             // SetWindowsHook and WH_*
#define NOWINOFFSETS     // GWL_*, GCL_*, associated routines
#define NOCOMM           // COMM driver routines
#define NOKANJI          // Kanji support stuff.
#define NOHELP           // Help engine interface.
#define NOPROFILER       // Profiler interface.
#define NODEFERWINDOWPOS // DeferWindowPos routines
#define NOMCX            // Modem Configuration Extensions
#define NOMINMAX

#include <Windows.h>
#include <WinUser.h>

#include <debugapi.h> // __debugbreak, OutputDebugString
                      // MiniDump {
#include <Dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")
// } MiniDump

// DirectX 11 {
#include <d3d11.h>       // D3D interface
#include <dxgi.h>        // DirectX driver interface
#include <d3dcompiler.h> // shader compiler

#pragma comment(lib, "d3d11.lib")       // direct3D library
#pragma comment(lib, "dxgi.lib")        // directx graphics interface
#pragma comment(lib, "d3dcompiler.lib") // shader compiler
// } DirectX 11

// ImGui Backend dependencies {
#include <windowsx.h> // GET_X_LPARAM(), GET_Y_LPARAM()
#include <tchar.h>
#include <dwmapi.h>
#include <xinput.h>
// } ImGui Backend dependencies

// near/far pointers O.O!
#undef near
#undef far
