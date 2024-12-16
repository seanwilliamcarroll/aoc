cmake_minimum_required(VERSION 3.20.2)

project(interpreters
  VERSION       0.1.0.0
  HOMEPAGE_URL  "https://github.com/seanwilliamcarroll/aoc24")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(Platform)

set(CMAKE_CXX_STANDARD            20 )
set(CMAKE_CXX_STANDARD_REQUIRED   ON )
set(CMAKE_CXX_EXTENSIONS          OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(iwyu_path NAMES include-what-you-use iwyu REQUIRED)
set(iwyu_path_and_options
    ${iwyu_path}
    -Xiwyu
    --error)

add_compile_options(-Wall -Werror)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options})
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

add_subdirectory(core)

add_subdirectory(d01)
add_subdirectory(d02)
add_subdirectory(d03)
add_subdirectory(d04)
add_subdirectory(d05)
add_subdirectory(d06)
add_subdirectory(d07)
add_subdirectory(d08)
add_subdirectory(d09)
add_subdirectory(d10)
add_subdirectory(d11)
add_subdirectory(d12)
add_subdirectory(d13)
add_subdirectory(d14)
add_subdirectory(d15)
add_subdirectory(d16)

# add_subdirectory(term_app)
