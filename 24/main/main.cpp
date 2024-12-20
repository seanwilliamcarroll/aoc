#include "core_lib.hpp" // for greet_day
#include AOC_DAY_FILE   // for part_1, part_2 // IWYU pragma: keep
#include <iostream>     // for char_traits, basic_ostream, operator<<, endl

#define MY_XSTR(a) MY_STR(a)
#define MY_STR(a) #a
#define AOC_DAY_ID_STR MY_XSTR(AOC_DAY_ID)

int main(int argc, char *argv[]) {
  greet_day(AOC_DAY_ID_STR);
  if (argc <= 1) {
    std::cerr << "Must provide filepath!" << std::endl;
    return -1;
  }

  std::cout << "Part 1: " << AOC_DAY_ID::part_1(argv[1]) << std::endl;

  std::cout << "Part 2: " << AOC_DAY_ID::part_2(argv[1]) << std::endl;

  return 0;
}

#undef MY_STR
#undef MY_XSTR
