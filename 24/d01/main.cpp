#include "core_lib.hpp" // for greet_day
#include <d01.hpp>      // for part_1
#include <iostream>     // for char_traits, basic_ostream, operator<<, endl

int main(int argc, char *argv[]) {
  greet_day(1);
  if (argc <= 1) {
    std::cerr << "Must provide filepath!" << std::endl;
    return -1;
  }

  std::cout << "Part 1: Total distance: " << d01::part_1(argv[1]) << std::endl;

  std::cout << "Part 2: Similarity score: " << d01::part_2(argv[1])
            << std::endl;

  return 0;
}
