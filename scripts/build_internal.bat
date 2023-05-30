@echo off
setlocal

SET CommonCompilerFlags=-std:c++20 -diagnostics:column -WL -O1 -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4127 -wd4201 -wd4324 -FC -Z7 -GS-
SET CommonCompilerFlags=-D_CRT_SECURE_NO_WARNINGS -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 %CommonCompilerFlags%
SET CommonLinkerFlags=-STACK:0x100000,0x100000 -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib kernel32.lib

SET GameCompilerFlags=/MT /Fo:build\ /Fe:build\rt_internal.exe code\first.cpp %CommonCompilerFlags%
REM GameLinkerFlags=/NODEFAULTLIB /SUBSYSTEM:windows
SET GameLinkerFlags=%CommonLinkerFlags%

rem Change this to your Visual Studio installation path if necessary
call "D:\Programs\vs-community-22\VC\Auxiliary\Build\vcvarsall.bat" x64

rem Create the build directory if it doesn't exist
if not exist build mkdir build

rem Delete old .exe
del build\rt_internal.exe

rem Compile and link game.cpp
cl %GameCompilerFlags% /link %GameLinkerFlags%

echo Build complete.
endlocal
