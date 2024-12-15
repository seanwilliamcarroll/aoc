
project(d15 VERSION 0.1.0)

add_executable(d15-exe main.cpp)

set_target_properties(d15-exe
  PROPERTIES
  OUTPUT_NAME d15
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)

target_link_libraries(d15-exe core)
