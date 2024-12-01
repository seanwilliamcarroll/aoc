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

# find_program(iwyu_path NAMES include-what-you-use iwyu REQUIRED)
# set(iwyu_path_and_options
#     ${iwyu_path}
#     -Xiwyu
#     --error)

add_compile_options(-Wall)

# set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options})
# set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

add_subdirectory(d01)

