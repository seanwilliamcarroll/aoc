#include <algorithm> // for max_element
#include <d19.hpp>
#include <fstream>   // for basic_istream, getline, basic_ifstream, basic_ios
#include <iostream>  // for cout
#include <set>       // for set, operator!=, __tree_const_iterator
#include <stddef.h>  // for size_t
#include <stdexcept> // for runtime_error
#include <string>    // for char_traits, string, to_string, basic_string
#include <utility>   // for make_pair, pair
#include <vector>    // for vector

namespace d19 {

using Towel = std::string;
using TowelPattern = std::string;

using Towels = std::vector<Towel>;
using TowelPatterns = std::vector<TowelPattern>;

TowelPatterns parse_towel_patterns(const std::string &line) {
  TowelPatterns towel_patterns;
  size_t pos{};

  while (pos < line.size()) {
    size_t comma = line.find(", ", pos);
    if (comma == std::string::npos) {
      comma = line.size();
    }
    towel_patterns.push_back(line.substr(pos, comma - pos));
    // skip over comma and space
    pos = comma + 2;
  }

  return towel_patterns;
}

std::pair<TowelPatterns, Towels>
get_towel_patterns_and_towels_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  std::string first_line;
  if (!std::getline(in_stream, first_line)) {
    throw std::runtime_error("Unexpected EOF!");
  }
  const auto towel_patterns = parse_towel_patterns(first_line);

  // get blank line
  std::string line;
  if (!std::getline(in_stream, line)) {
    throw std::runtime_error("Unexpected EOF!");
  } else if (!line.empty()) {
    throw std::runtime_error("Expected blank line!!");
  }

  Towels towels;
  while (std::getline(in_stream, line)) {
    towels.push_back(line);
  }

  return std::make_pair(towel_patterns, towels);
}

template <typename Element> void print_vector(const std::vector<Element> &vec) {
  for (const auto &elem : vec) {
    std::cout << elem << " ";
  }
  std::cout << std::endl;
}

size_t num_designs_possible(const std::set<TowelPattern> &unique_patterns,
                            const size_t max_length_pattern,
                            const Towel &towel) {
  std::vector<size_t> counts(towel.size() + 1, 0);
  counts[0] = 1;

  for (size_t index = 1; index < towel.size() + 1; ++index) {
    // Look at all previous pattern lengths, if the last n chars are a pattern,
    // add to this count
    for (size_t length = 1; length <= max_length_pattern && length <= index;
         ++length) {
      const auto sub_string = towel.substr(index - length, length);
      if (unique_patterns.count(sub_string) > 0) {
        counts[index] += counts[index - length];
      }
    }
  }

  return counts.back();
}

size_t get_number_possible_designs(const TowelPatterns &towel_patterns,
                                   const Towels &towels) {
  size_t number_possible{};

  const std::set<TowelPattern> unique_patterns(towel_patterns.begin(),
                                               towel_patterns.end());
  std::set<size_t> pattern_lengths;
  for (const auto &pattern : towel_patterns) {
    pattern_lengths.insert(pattern.size());
  }
  const size_t max_length_pattern =
      *std::max_element(pattern_lengths.begin(), pattern_lengths.end());

  for (const auto &towel : towels) {
    if (num_designs_possible(unique_patterns, max_length_pattern, towel) > 0) {
      ++number_possible;
    }
  }

  return number_possible;
}

size_t get_total_number_possible_designs(const TowelPatterns &towel_patterns,
                                         const Towels &towels) {
  size_t number_possible{};

  const std::set<TowelPattern> unique_patterns(towel_patterns.begin(),
                                               towel_patterns.end());
  std::set<size_t> pattern_lengths;
  for (const auto &pattern : towel_patterns) {
    pattern_lengths.insert(pattern.size());
  }
  const size_t max_length_pattern =
      *std::max_element(pattern_lengths.begin(), pattern_lengths.end());

  for (const auto &towel : towels) {
    number_possible +=
        num_designs_possible(unique_patterns, max_length_pattern, towel);
  }

  return number_possible;
}

std::string part_1(const std::string &filepath) {

  const auto [towel_patterns, towels] =
      get_towel_patterns_and_towels_from_file(filepath);

  const auto num_possible = get_number_possible_designs(towel_patterns, towels);

  return std::to_string(num_possible);
}

std::string part_2(const std::string &filepath) {

  const auto [towel_patterns, towels] =
      get_towel_patterns_and_towels_from_file(filepath);

  const auto num_possible =
      get_total_number_possible_designs(towel_patterns, towels);

  return std::to_string(num_possible);
}

} // namespace d19
