
project(d18 VERSION 0.1.0)

add_executable(d18-exe main.cpp)

set_target_properties(d18-exe
  PROPERTIES
  OUTPUT_NAME d18
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)

target_link_libraries(d18-exe core)
