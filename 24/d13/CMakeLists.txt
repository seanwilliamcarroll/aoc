
project(d13 VERSION 0.1.0)

add_executable(d13-exe main.cpp)

set_target_properties(d13-exe
  PROPERTIES
  OUTPUT_NAME d13
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)

target_link_libraries(d13-exe core)
