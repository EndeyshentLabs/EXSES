cmake_minimum_required(VERSION 3.14)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_CXX_STANDARD 20)

project(EXSES CXX)

add_executable(
    exsi
    src/main.cpp
    src/Lexer.cpp
    src/utils.cpp
    src/Procedure.cpp
    src/Token.cpp
    src/Value.cpp
)

target_include_directories(exsi PUBLIC src)

target_compile_options(exsi PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Werror=switch
    -fdiagnostics-color
    $<$<CONFIG:DEBUG>:-O0 -g3 -ggdb3 -DDEBUG>
    $<$<CONFIG:RELEASE>:-O3>
)

target_link_options(exsi PRIVATE
    $<$<CONFIG:DEBUG>:-O0 -g3 -ggdb3>
    $<$<CONFIG:RELEASE>:-O3>
)
