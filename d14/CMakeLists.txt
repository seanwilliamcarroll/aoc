
project(d14 VERSION 0.1.0)

add_executable(d14-exe main.cpp)

set_target_properties(d14-exe
  PROPERTIES
  OUTPUT_NAME d14
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)
