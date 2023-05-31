cmake_minimum_required(VERSION 3.14)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_CXX_STANDARD 20)

project(xesvm CXX)

add_executable(
    xesasm
    src/main.cpp
    src/Xesvm.cpp
)
target_include_directories(xesasm PUBLIC src)
target_compile_options(xesasm PRIVATE -Wall -Wextra -pedantic -Wswitch-enum -fdiagnostics-color)
