project(core VERSION 0.0.1)

add_library(core STATIC
  src/core_lib.cpp
)

set_target_properties(core
  PROPERTIES
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)

target_include_directories(core PUBLIC inc)
