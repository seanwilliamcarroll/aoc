
project(d01 VERSION 0.1.0)

add_executable(d01-exe main.cpp)

set_target_properties(d01-exe
  PROPERTIES
  OUTPUT_NAME d01
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)

target_link_libraries(d01-exe core)
