
project(d08 VERSION 0.1.0)

add_executable(d08-exe main.cpp)

set_target_properties(d08-exe
  PROPERTIES
  OUTPUT_NAME d08
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)

target_link_libraries(d08-exe core)
