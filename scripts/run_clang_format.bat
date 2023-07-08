@echo off
REM Script for formatting all source files except 3rdparty.

for /R "../code/" %%G in (*.cxx *.hxx *.cpp *.hpp) do (
    echo Processing %%G
    echo %%G | findstr /C:"code\\3rdparty" 1>nul
    if errorlevel 1 (
        clang-format -i "%%G"
    )
)
