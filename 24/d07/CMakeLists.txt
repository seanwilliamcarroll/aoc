
project(d07 VERSION 0.1.0)

add_executable(d07-exe main.cpp)

set_target_properties(d07-exe
  PROPERTIES
  OUTPUT_NAME d07
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)

target_link_libraries(d07-exe core)
