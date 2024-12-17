
project(d16 VERSION 0.1.0)

add_executable(d16-exe main.cpp)

set_target_properties(d16-exe
  PROPERTIES
  OUTPUT_NAME d16
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)

target_link_libraries(d16-exe core)
