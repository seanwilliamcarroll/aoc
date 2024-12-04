#include <_stdlib.h>     // for abs
#include <algorithm>     // for sort
#include <cmath>         // for abs
#include <cstdlib>       // for size_t
#include <fstream>       // for basic_ostream, endl, operator<<, basic_istream
#include <iostream>      // for cout, cerr
#include <string>        // for char_traits, string
#include <unordered_map> // for unordered_map
#include <utility>       // for make_pair, pair
#include <vector>        // for vector

std::pair<std::vector<int>, std::vector<int>>
get_two_lists_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  std::vector<int> list_0;
  std::vector<int> list_1;

  int x0 = 0;
  int x1 = 0;

  while (in_stream >> x0 >> x1) {
    list_0.push_back(x0);
    list_1.push_back(x1);
  }
  return std::make_pair(list_0, list_1);
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    std::cerr << "Must provide filepath!" << std::endl;
    return -1;
  }
  auto [list_0, list_1] = get_two_lists_from_file(argv[1]);

  std::sort(list_0.begin(), list_0.end());
  std::sort(list_1.begin(), list_1.end());

  int accumulator = 0;
  for (size_t index = 0; index < list_0.size(); ++index) {
    accumulator += std::abs(list_0[index] - list_1[index]);
  }

  std::cout << "Total distance: " << accumulator << std::endl;

  std::unordered_map<int, size_t> list_1_counts;
  for (const auto elem : list_1) {
    list_1_counts[elem] += 1;
  }

  accumulator = 0;
  for (const auto elem : list_0) {
    accumulator += (elem * list_1_counts[elem]);
  }

  std::cout << "Similarity score: " << accumulator << std::endl;

  return 0;
}
