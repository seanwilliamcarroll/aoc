
project(tests VERSION 0.1.0)

enable_testing()

add_executable(all_tests-exe main.cpp)

set_target_properties(all_tests-exe
  PROPERTIES
  OUTPUT_NAME all_tests
  CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options}
)

target_link_libraries(all_tests-exe
  d01-lib
  GTest::gtest_main
)

target_compile_definitions(all_tests-exe PUBLIC AOC_TOP_DIR="${CMAKE_SOURCE_DIR}")


include(GoogleTest)
gtest_discover_tests(all_tests-exe)
