#include <d01.hpp>       // for part_1, part_2
#include <fstream>       // for basic_ifstream, getline, basic_ostream, endl
#include <gtest/gtest.h> // for Test, Message, EXPECT_EQ, TestInfo (ptr only)
#include <iostream>      // for cout
#include <string>        // for char_traits, operator+, string, basic_string
#include <utility>       // for make_pair, pair

std::pair<std::string, std::string> get_answers(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  std::cout << filepath << std::endl;

  std::string part_1;
  std::getline(in_stream, part_1);

  std::string part_2;
  std::getline(in_stream, part_2);

  return std::make_pair(part_1, part_2);
}

// Demonstrate some basic assertions.
TEST(Daily, D01) {
  std::string full_filepath(AOC_TOP_DIR);
  full_filepath += "/";
  const auto part_1 = d01::part_1(full_filepath + "d01/input.txt");
  const auto part_2 = d01::part_2(full_filepath + "d01/input.txt");

  const auto [part_1_expected, part_2_expected] =
      get_answers(full_filepath + "d01/answer.txt");

  EXPECT_EQ(part_1, part_1_expected);

  EXPECT_EQ(part_2, part_2_expected);
}
