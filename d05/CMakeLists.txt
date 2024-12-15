
project(d05 VERSION 0.1.0)

add_executable(d05-exe main.cpp)

set_target_properties(d05-exe
  PROPERTIES
  OUTPUT_NAME d05
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)

target_link_libraries(d05-exe core)
