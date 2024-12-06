
project(d06 VERSION 0.1.0)

add_executable(d06-exe main.cpp)

set_target_properties(d06-exe
  PROPERTIES
  OUTPUT_NAME d06
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)
