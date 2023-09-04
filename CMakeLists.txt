# CMake minimum required version
cmake_minimum_required(VERSION 3.10)

# Project name
set(PROJECT rt)
project(${PROJECT})

# C++ standard
set(CMAKE_CXX_STANDARD 20)

# Compiler Flags
set(COMMON_COMPILER_FLAGS "-std:c++20 -diagnostics:column -WL -O2 -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4127 -wd4201 -wd4324 -FC -Z7 -GS-")
set(COMMON_COMPILER_FLAGS "${COMMON_COMPILER_FLAGS} -D_CRT_SECURE_NO_WARNINGS -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILER_FLAGS}")

# Linker Flags
set(COMMON_LINKER_FLAGS "-STACK:0x100000,0x100000 -incremental:no -opt:ref -profile")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${COMMON_LINKER_FLAGS}")

# Linker Libraries
set(LINKER_LIBS user32.lib gdi32.lib winmm.lib kernel32.lib)
link_libraries(${LINKER_LIBS})

# Include and Source files
include_directories(${CMAKE_SOURCE_DIR}/code) # Assuming code is in a directory named "code"
set(SOURCES code/first.cpp)

# Output paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# Executable
add_executable(${PROJECT} ${SOURCES})

# Set the working directory for the debugger (Visual Studio specific)
set_target_properties(${PROJECT} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/run_tree"
)
