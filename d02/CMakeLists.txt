
project(d02 VERSION 0.1.0)

add_executable(d02-exe main.cpp)

set_target_properties(d02-exe
  PROPERTIES
  OUTPUT_NAME d02
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)
