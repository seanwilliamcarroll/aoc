
project(d11 VERSION 0.1.0)

add_executable(d11-exe main.cpp)

set_target_properties(d11-exe
  PROPERTIES
  OUTPUT_NAME d11
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)
