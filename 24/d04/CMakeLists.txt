
project(d04 VERSION 0.1.0)

add_executable(d04-exe main.cpp)

set_target_properties(d04-exe
  PROPERTIES
  OUTPUT_NAME d04
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)

target_link_libraries(d04-exe core)
