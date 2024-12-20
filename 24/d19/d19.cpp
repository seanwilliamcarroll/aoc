#include <d19.hpp>
#include <deque>     // for deque
#include <fstream>   // for basic_istream, getline, basic_ifstream, basic_ios
#include <set>       // for set
#include <stddef.h>  // for size_t
#include <stdexcept> // for runtime_error
#include <string>    // for string, char_traits, basic_string, to_string
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

bool is_design_possible(const TowelPatterns &towel_patterns,
                        const Towel &towel) {
  std::deque<size_t> positions;
  positions.push_back(0);

  std::set<size_t> seen;

  while (!positions.empty()) {
    size_t pos = positions.front();
    positions.pop_front();
    if (pos == towel.size()) {
      return true;
    }
    if (seen.count(pos) > 0) {
      continue;
    }
    for (const auto &pattern : towel_patterns) {
      if (pattern.size() > towel.size() - pos) {
        continue;
      }
      size_t index{};
      while (index < pattern.size() && pattern[index] == towel[pos + index]) {
        ++index;
      }
      if (index == pattern.size()) {
        positions.push_back(pos + pattern.size());
      }
    }
    seen.insert(pos);
  }

  return false;
}

size_t get_number_possible_designs(const TowelPatterns &towel_patterns,
                                   const Towels &towels) {
  size_t number_possible{};

  for (const auto &towel : towels) {
    if (is_design_possible(towel_patterns, towel)) {
      ++number_possible;
    }
  }

  return number_possible;
}

std::string part_1(const std::string &filepath) {

  const auto [towel_patterns, towels] =
      get_towel_patterns_and_towels_from_file(filepath);

  const auto num_possible = get_number_possible_designs(towel_patterns, towels);

  return std::to_string(num_possible);
}

std::string part_2(const std::string &filepath) { return std::string(); }

} // namespace d19
