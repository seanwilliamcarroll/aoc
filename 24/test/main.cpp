#include <d01.hpp>       // for part_1, part_2
#include <d02.hpp>       // for part_1, part_2
#include <d03.hpp>       // for part_1, part_2
#include <d04.hpp>       // for part_1, part_2
#include <d05.hpp>       // for part_1, part_2
#include <d06.hpp>       // for part_1, part_2
#include <d07.hpp>       // for part_1, part_2
#include <d08.hpp>       // for part_1, part_2
#include <d09.hpp>       // for part_1, part_2
#include <d10.hpp>       // for part_1, part_2
#include <d11.hpp>       // for part_1, part_2
#include <d12.hpp>       // for part_1, part_2
#include <d13.hpp>       // for part_1, part_2
#include <d14.hpp>       // for part_1, part_2
#include <d15.hpp>       // for part_1, part_2
#include <d16.hpp>       // for part_1, part_2
#include <d17.hpp>       // for part_1, part_2
#include <d18.hpp>       // for part_1, part_2
#include <d19.hpp>       // for part_1, part_2
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

#define MY_XSTR(a) MY_STR(a)
#define MY_STR(a) #a

#define MY_TEST(DAY_ID)                                                        \
  std::string full_filepath(AOC_TOP_DIR);                                      \
  full_filepath += "/";                                                        \
  const auto part_1 =                                                          \
      d##DAY_ID::part_1(full_filepath + "d" MY_XSTR(DAY_ID) "/input.txt");     \
  const auto part_2 =                                                          \
      d##DAY_ID::part_2(full_filepath + "d" MY_XSTR(DAY_ID) "/input.txt");     \
                                                                               \
  const auto [part_1_expected, part_2_expected] =                              \
      get_answers(full_filepath + "d" MY_XSTR(DAY_ID) "/answer.txt");          \
                                                                               \
  EXPECT_EQ(part_1, part_1_expected);                                          \
                                                                               \
  EXPECT_EQ(part_2, part_2_expected);

TEST(Daily, D01) { MY_TEST(01); }

TEST(Daily, D02) { MY_TEST(02); }

TEST(Daily, D03) { MY_TEST(03); }

TEST(Daily, D04) { MY_TEST(04); }

TEST(Daily, D05) { MY_TEST(05); }

TEST(Daily, D06) { MY_TEST(06); }

TEST(Daily, D07) { MY_TEST(07); }

TEST(Daily, D08) { MY_TEST(08); }

TEST(Daily, D09) { MY_TEST(09); }

TEST(Daily, D10) { MY_TEST(10); }

TEST(Daily, D11) { MY_TEST(11); }

TEST(Daily, D12) { MY_TEST(12); }

TEST(Daily, D13) { MY_TEST(13); }

TEST(Daily, D14) { MY_TEST(14); }

TEST(Daily, D15) { MY_TEST(15); }

TEST(Daily, D16) { MY_TEST(16); }

TEST(Daily, D17) { MY_TEST(17); }

TEST(Daily, D18) { MY_TEST(18); }

TEST(Daily, D19) { MY_TEST(19); }

#undef MY_XSTR
#undef MY_STR
#undef MY_TEST
