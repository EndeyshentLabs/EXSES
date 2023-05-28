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
)

target_include_directories(exsi PUBLIC src)

target_compile_options(exsi PRIVATE -Wall -Wextra -O2 -pedantic -Werror=switch -fdiagnostics-color)

# TODO: Make debug compilation target. Stub:
# add_custom_target(debug)
# if(TARGET debug)
#     target_compile_definitions(exsi PUBLIC -DPROCEDURE_DEBUG)
# endif()
# add_dependencies(debug exsi)
