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

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        b514bdc898e2951020cbdca1304b75f5950d1f59
)
FetchContent_MakeAvailable(googletest)


enable_testing()

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


function(make_my_day)
  project(d${ARGV0} VERSION 0.1.0)

  add_library(d${ARGV0}-lib d${ARGV0}.cpp)

  target_include_directories(d${ARGV0}-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}) 

  target_link_libraries(d${ARGV0}-lib core)

  set_target_properties(d${ARGV0}-lib
    PROPERTIES
    OUTPUT_NAME d${ARGV0}-lib
    CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
  )

  add_executable(d${ARGV0}-exe ${CMAKE_SOURCE_DIR}/main/main.cpp)

  set(AOC_DAY_ID d${ARGV0})

  target_compile_definitions(d${ARGV0}-exe PUBLIC
    AOC_DAY_ID=${AOC_DAY_ID}
    AOC_DAY_FILE="${AOC_DAY_ID}.hpp"
  )

  set_target_properties(d${ARGV0}-exe
    PROPERTIES
    OUTPUT_NAME d${ARGV0}
    CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
  )

  target_link_libraries(d${ARGV0}-exe d${ARGV0}-lib)
  
endfunction()

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
add_subdirectory(d17)
add_subdirectory(d18)
add_subdirectory(d19)
add_subdirectory(d20)
add_subdirectory(d21)

add_subdirectory(test)

# add_subdirectory(term_app)
