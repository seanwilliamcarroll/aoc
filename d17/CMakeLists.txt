
project(d17 VERSION 0.1.0)

add_executable(d17-exe main.cpp)

set_target_properties(d17-exe
  PROPERTIES
  OUTPUT_NAME d17
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)

target_link_libraries(d17-exe core)
