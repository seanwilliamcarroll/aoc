#include <array>        // for array
#include <core_lib.hpp> // for Grid, Tile
#include <d25.hpp>
#include <fstream>  // for basic_istream, basic_ifstream, basic_ostream
#include <iostream> // for cout
#include <map>      // for map, __map_iterator
#include <set>      // for set
#include <stddef.h> // for size_t
#include <string>   // for char_traits, string, basic_string, to_string
#include <utility>  // for make_pair, pair
#include <vector>   // for vector

// clang-format off
namespace d25 {
// clang-format on

using KeyMap = Grid;
using LockMap = Grid;

using KeyMaps = std::vector<KeyMap>;
using LockMaps = std::vector<LockMap>;

constexpr size_t NUM_TUMBLERS = 5;
constexpr size_t MAX_TUMBLER_HEIGHT = 5;

using TumblerHeights = std::array<size_t, NUM_TUMBLERS>;

constexpr Tile SOLID_TILE = '#';
constexpr Tile EMPTY_TILE = '#';

TumblerHeights operator+(const TumblerHeights &left,
                         const TumblerHeights &right) {
  TumblerHeights out{};
  for (size_t index{}; index < NUM_TUMBLERS; ++index) {
    out[index] = left[index] + right[index];
  }
  return out;
}

bool is_valid_tumbler(const TumblerHeights &tumbler) {
  for (size_t index{}; index < NUM_TUMBLERS; ++index) {
    if (tumbler[index] > MAX_TUMBLER_HEIGHT) {
      return false;
    }
  }
  return true;
}

std::pair<KeyMaps, LockMaps>
get_key_and_lock_maps_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  bool parsing_key = false;
  bool parsing_lock = false;
  KeyMaps keys;
  LockMaps locks;
  KeyMap current_key;
  LockMap current_lock;
  for (std::string line; std::getline(in_stream, line); /*BLANK*/) {
    if (line.size() == 0) {
      if (parsing_lock) {
        locks.push_back(current_lock);
        current_lock = LockMap{};
      } else {
        keys.push_back(current_key);
        current_key = KeyMap{};
      }
      parsing_key = false;
      parsing_lock = false;
      continue;
    }
    if (!parsing_lock && !parsing_key) {
      if (line[0] == SOLID_TILE) {
        parsing_lock = true;
      } else {
        parsing_key = true;
      }
    }
    if (parsing_lock) {
      current_lock.push_back(line);
    } else {
      current_key.push_back(line);
    }
  }
  if (parsing_lock) {
    locks.push_back(current_lock);
  } else if (parsing_key) {
    keys.push_back(current_key);
  }
  return std::make_pair(keys, locks);
}

TumblerHeights get_key_tumbler_heights(const KeyMap &key) {
  TumblerHeights output{};
  for (size_t index{}; index < key.size() - 1; ++index) {
    const auto &line = key[index];
    for (size_t char_index{}; char_index < line.size(); ++char_index) {
      if (line[char_index] == SOLID_TILE) {
        ++output[char_index];
      }
    }
  }
  return output;
}

TumblerHeights get_lock_tumbler_heights(const LockMap &lock) {
  TumblerHeights output{};
  for (size_t index = 1; index < lock.size(); ++index) {
    const auto &line = lock[index];
    for (size_t char_index{}; char_index < line.size(); ++char_index) {
      if (line[char_index] == EMPTY_TILE) {
        ++output[char_index];
      }
    }
  }
  return output;
}

void print_tumbler_heights(const TumblerHeights &tumbler_heights) {
  for (const auto val : tumbler_heights) {
    std::cout << val << " ";
  }
  std::cout << std::endl;
}

size_t get_num_unique_matches(const KeyMaps &keys, const LockMaps &locks) {

  std::map<TumblerHeights, std::set<TumblerHeights>> keys_to_locks;

  std::vector<TumblerHeights> key_tumblers;
  std::vector<TumblerHeights> lock_tumblers;

  for (const auto &key : keys) {
    key_tumblers.push_back(get_key_tumbler_heights(key));
  }

  for (const auto &lock : locks) {
    lock_tumblers.push_back(get_lock_tumbler_heights(lock));
  }

  for (const auto &key_tumbler : key_tumblers) {
    for (const auto &lock_tumbler : lock_tumblers) {
      TumblerHeights attempt = key_tumbler + lock_tumbler;
      if (is_valid_tumbler(attempt)) {
        keys_to_locks[key_tumbler].insert(lock_tumbler);
      }
    }
  }
  size_t num_valid{};
  for (const auto &[key, valid_locks] : keys_to_locks) {
    num_valid += valid_locks.size();
  }
  return num_valid;
}

std::string part_1(const std::string &filepath) {

  const auto [keys, locks] = get_key_and_lock_maps_from_file(filepath);

  size_t num_unique = get_num_unique_matches(keys, locks);

  return std::to_string(num_unique);
}

std::string part_2(const std::string &filepath) {
  return std::string("Delivered!");
}

// clang-format off
} // namespace d25
// clang-format on
