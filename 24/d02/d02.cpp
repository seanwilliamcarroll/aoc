#include <_stdlib.h> // for abs
#include <cmath>     // IWYU pragma: keep
#include <cstdlib>   // for size_t
#include <d02.hpp>
#include <fstream> // for basic_istream, basic_ifstream, getline, ifstream
#include <string>  // for char_traits, string, stoi, to_string
#include <vector>  // for vector

namespace d02 {
std::vector<std::vector<int>> get_lists_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  std::vector<std::vector<int>> output;

  for (std::string line; std::getline(in_stream, line); /*BLANK*/) {
    std::vector<int> line_vec;
    size_t pos = 0;
    while (line.size() > 0) {
      int value{std::stoi(line, &pos)};
      line_vec.push_back(value);
      line.erase(0, pos);
    }
    output.push_back(line_vec);
  }
  return output;
}

constexpr bool SAFE = true;
constexpr bool UNSAFE = false;

bool is_safe(const std::vector<int> &list) {
  int last_val = 0;
  bool all_increasing = SAFE;
  bool all_decreasing = SAFE;

  for (size_t index = 0; index < list.size(); ++index) {
    if (index == 0) {
      last_val = list[index];
      continue;
    }
    const auto curr_val = list[index];
    const auto abs_diff = std::abs(curr_val - last_val);
    if (abs_diff < 1 || abs_diff > 3) {
      return UNSAFE;
    }
    if (curr_val >= last_val) {
      all_decreasing = UNSAFE;
    }
    if (curr_val <= last_val) {
      all_increasing = UNSAFE;
    }
    last_val = list[index];
  }
  return all_increasing || all_decreasing;
}

bool is_safe_with_dampener(const std::vector<int> &list) {
  if (is_safe(list)) {
    return SAFE;
  }

  std::vector<int> list_copy;
  list_copy.reserve(list.size());

  for (size_t index_to_remove = 0; index_to_remove < list.size();
       ++index_to_remove) {
    for (size_t index = 0; index < list.size(); ++index) {
      if (index == index_to_remove) {
        continue;
      }
      list_copy.push_back(list[index]);
    }
    if (is_safe(list_copy)) {
      return SAFE;
    }
    list_copy.clear();
  }
  return UNSAFE;
}

std::string part_1(const std::string &filepath) {
  auto lists = get_lists_from_file(filepath);

  int accumulator = 0;
  for (const auto &list : lists) {
    accumulator += int(is_safe(list));
  }

  return std::to_string(accumulator);
}

std::string part_2(const std::string &filepath) {
  auto lists = get_lists_from_file(filepath);
  int accumulator = 0;
  for (const auto &list : lists) {
    accumulator += int(is_safe_with_dampener(list));
  }
  return std::to_string(accumulator);
}

} // namespace d02
