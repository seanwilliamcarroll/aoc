
project(d12 VERSION 0.1.0)

add_executable(d12-exe main.cpp)

set_target_properties(d12-exe
  PROPERTIES
  OUTPUT_NAME d12
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)

target_link_libraries(d12-exe core)
