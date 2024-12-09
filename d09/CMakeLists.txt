
project(d09 VERSION 0.1.0)

add_executable(d09-exe main.cpp)

set_target_properties(d09-exe
  PROPERTIES
  OUTPUT_NAME d09
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)
