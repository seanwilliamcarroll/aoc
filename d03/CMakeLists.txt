
project(d03 VERSION 0.1.0)

add_executable(d03-exe main.cpp)

set_target_properties(d03-exe
  PROPERTIES
  OUTPUT_NAME d03
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)

target_link_libraries(d03-exe core)
