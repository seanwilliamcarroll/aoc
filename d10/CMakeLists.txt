
project(d10 VERSION 0.1.0)

add_executable(d10-exe main.cpp)

set_target_properties(d10-exe
  PROPERTIES
  OUTPUT_NAME d10
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)
