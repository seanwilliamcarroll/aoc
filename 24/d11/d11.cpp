#include <cmath> // for floor, log10, pow
#include <d11.hpp>
#include <fstream>  // for basic_istream, basic_ifstream, ifstream
#include <map>      // for __map_const_iterator, map
#include <stddef.h> // for size_t
#include <string>   // for string, to_string
#include <utility>  // for make_pair, pair
#include <vector>   // for vector

namespace d11 {
using Stone = long long;
using Stones = std::vector<Stone>;

using UniqueStoneCounts = std::map<Stone, size_t>;

Stones get_stones_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  Stones stones;

  Stone stone{};

  while (in_stream >> stone) {
    stones.push_back(stone);
  }

  return stones;
}

std::pair<Stone, Stone> do_blink(const Stone stone) {
  if (stone == 0) {
    return std::make_pair(1, -1);
  }
  int num_digits = int(std::floor(std::log10(stone))) + 1;
  if (num_digits % 2 == 0) {
    Stone divisor = std::pow(10, num_digits / 2);
    Stone new_stone_0 = stone / divisor;
    Stone new_stone_1 = stone % divisor;
    return std::make_pair(new_stone_0, new_stone_1);
  }
  Stone new_stone = stone * 2024;
  return std::make_pair(new_stone, -1);
}

UniqueStoneCounts blink(const UniqueStoneCounts &orig_counts) {
  UniqueStoneCounts new_counts;
  for (const auto [stone, count] : orig_counts) {
    const auto [new_stone_0, new_stone_1] = do_blink(stone);
    new_counts[new_stone_0] += count;
    if (new_stone_1 >= 0) {
      new_counts[new_stone_1] += count;
    }
  }
  return new_counts;
}

size_t get_total_count(const UniqueStoneCounts &counts) {
  size_t count{};
  for (const auto &iter : counts) {
    count += iter.second;
  }
  return count;
}

UniqueStoneCounts do_n_blinks(const UniqueStoneCounts &counts,
                              const size_t n_blinks) {
  UniqueStoneCounts out_counts(counts);
  for (size_t index = 0; index < n_blinks; ++index) {
    out_counts = blink(out_counts);
  }
  return out_counts;
}

UniqueStoneCounts create_counts(const Stones &stones) {
  UniqueStoneCounts counts;
  for (const auto stone : stones) {
    counts[stone] += 1;
  }
  return counts;
}

std::string part_1(const std::string &filepath) {

  const auto stones = get_stones_from_file(filepath);

  UniqueStoneCounts counts = create_counts(stones);

  counts = do_n_blinks(counts, 25);

  return std::to_string(get_total_count(counts));
}

std::string part_2(const std::string &filepath) {

  const auto stones = get_stones_from_file(filepath);

  UniqueStoneCounts counts = create_counts(stones);

  counts = do_n_blinks(counts, 75);

  return std::to_string(get_total_count(counts));
}

} // namespace d11
