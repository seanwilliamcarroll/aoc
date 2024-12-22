#include <core_lib.hpp> // for Position, Tile, get_lines_from_file
#include <d21.hpp>
#include <map>      // for map, __map_const_iterator, operator!=
#include <stddef.h> // for size_t
#include <string>   // for operator+, basic_string, string, to_string
#include <utility>  // for pair, make_pair, operator==
#include <vector>   // for vector

// clang-format off
namespace d21 {
// clang-format on

using Sequence = std::string;
using Sequences = std::vector<Sequence>;

using Movement = std::pair<Position, Position>;
using MemoizationKey = std::pair<Movement, size_t>;

constexpr Tile INVALID = 99;

const std::map<Tile, Position> NUMBER_PAD_POSITIONS = {
    {'7', {0, 0}}, {'8', {0, 1}},     {'9', {0, 2}}, {'4', {1, 0}},
    {'5', {1, 1}}, {'6', {1, 2}},     {'1', {2, 0}}, {'2', {2, 1}},
    {'3', {2, 2}}, {INVALID, {3, 0}}, {'0', {3, 1}}, {'A', {3, 2}}};

const std::map<Tile, Position> DIRECTION_PAD_POSITIONS = {{{INVALID, {0, 0}},
                                                           {'^', {0, 1}},
                                                           {'A', {0, 2}},
                                                           {'<', {1, 0}},
                                                           {'v', {1, 1}},
                                                           {'>', {1, 2}}}};

constexpr Tile ENTER_KEY = 'A';
constexpr Sequence ENTER_SEQUENCE = "A";

const std::map<Tile, Position> DIRECTION_INCREMENTS = {
    {{'^', {-1, 0}}, {'>', {0, 1}}, {'<', {0, -1}}, {'v', {1, 0}}}};

Sequence get_sequence(const Position &initial_position,
                      const Position &target_position,
                      const std::map<Tile, Position> &pad_positions) {
  const auto &[target_row, target_col] = target_position;
  const auto &[initial_row, initial_col] = initial_position;

  const auto invalid_position = pad_positions.at(INVALID);

  // Need to try both row and column as priority to see which gives us the
  // faster solution??

  // Do we only need to move in one direction
  if (target_row == initial_row && target_col == initial_col) {
    return ENTER_SEQUENCE;
  } else if (target_row == initial_row) {
    if (initial_col < target_col) {
      return Sequence(target_col - initial_col, '>') + ENTER_SEQUENCE;
    } else {
      return Sequence(initial_col - target_col, '<') + ENTER_SEQUENCE;
    }
  } else if (target_col == initial_col) {
    if (initial_row < target_row) {
      return Sequence(target_row - initial_row, 'v') + ENTER_SEQUENCE;
    } else {
      return Sequence(initial_row - target_row, '^') + ENTER_SEQUENCE;
    }
  }
  // We need to do both directions
  Sequence vertical;
  if (initial_col < target_col) {
    vertical = Sequence(target_col - initial_col, '>');
  } else {
    vertical = Sequence(initial_col - target_col, '<');
  }

  Sequence horizontal;
  if (initial_row < target_row) {
    horizontal = Sequence(target_row - initial_row, 'v');
  } else {
    horizontal = Sequence(initial_row - target_row, '^');
  }
  // Will we hit invalid?
  if (std::make_pair(initial_row, target_col) == invalid_position) {
    // Going vertical first will hit invalid
    return horizontal + vertical + ENTER_SEQUENCE;
  } else if (std::make_pair(target_row, initial_col) == invalid_position) {
    // Going horizontal first will hit invalid
    return vertical + horizontal + ENTER_SEQUENCE;
  }

  if (initial_row > target_row && initial_col > target_col) {
    return vertical + horizontal + ENTER_SEQUENCE;
  } else if (initial_row > target_row && initial_col < target_col) {
    return horizontal + vertical + ENTER_SEQUENCE;
  } else if (initial_row < target_row && initial_col > target_col) {
    return vertical + horizontal + ENTER_SEQUENCE;
  } else { // if (initial_row < target_row && initial_col < target_col) {
    return horizontal + vertical + ENTER_SEQUENCE;
  }
}

std::map<Movement, Sequence>
calc_lookup_table(const std::map<Tile, Position> &pad_positions) {
  std::map<Movement, Sequence> lookup_table;
  for (const auto &[start_tile, start_position] : pad_positions) {
    if (start_tile == INVALID) {
      continue;
    }
    for (const auto &[target_tile, target_position] : pad_positions) {
      if (target_tile == INVALID) {
        continue;
      }
      const auto movement = std::make_pair(start_position, target_position);
      if (start_position == target_position) {
        lookup_table[movement] = ENTER_SEQUENCE;
        continue;
      }
      lookup_table[movement] =
          get_sequence(start_position, target_position, pad_positions);
    }
  }
  return lookup_table;
}

const auto NUMBER_MOVEMENT_TO_SEQUENCE =
    calc_lookup_table(NUMBER_PAD_POSITIONS);

const auto DIRECTION_MOVEMENT_TO_SEQUENCE =
    calc_lookup_table(DIRECTION_PAD_POSITIONS);

size_t get_numeric_part(const Sequence &sequence) {
  size_t output{};
  for (const auto character : sequence) {
    if (!(character >= '0' && character <= '9')) {
      continue;
    }
    output *= 10;
    output += character - '0';
  }
  return output;
}

std::map<MemoizationKey, size_t> previous_lengths;

size_t calc_length_at_level(const Position &start_position,
                            const Position &target_position,
                            const size_t num_robots) {

  const auto movement = std::make_pair(start_position, target_position);
  const auto key = std::make_pair(movement, num_robots);

  const auto &iter = previous_lengths.find(key);
  if (iter != previous_lengths.end()) {
    return iter->second;
  }

  if (num_robots == 0) {
    const auto output = DIRECTION_MOVEMENT_TO_SEQUENCE.at(movement).size();
    previous_lengths[key] = output;
    return output;
  }
  const auto sequence = DIRECTION_MOVEMENT_TO_SEQUENCE.at(movement);
  size_t sum{};
  auto last_position = DIRECTION_PAD_POSITIONS.at(ENTER_KEY);
  for (const auto tile : sequence) {
    auto next_position = DIRECTION_PAD_POSITIONS.at(tile);
    sum += calc_length_at_level(last_position, next_position, num_robots - 1);
    last_position = next_position;
  }
  previous_lengths[key] = sum;
  return sum;
}

size_t get_total_length(const Sequence &sequence, size_t num_robots) {
  auto last_position = NUMBER_PAD_POSITIONS.at(ENTER_KEY);
  size_t sum{};
  auto last_dir_position = DIRECTION_PAD_POSITIONS.at(ENTER_KEY);
  for (const auto tile : sequence) {
    auto next_position = NUMBER_PAD_POSITIONS.at(tile);
    auto movement = std::make_pair(last_position, next_position);
    const auto number_sequence = NUMBER_MOVEMENT_TO_SEQUENCE.at(movement);
    for (const auto sequence_tile : number_sequence) {
      auto next_dir_position = DIRECTION_PAD_POSITIONS.at(sequence_tile);
      sum += calc_length_at_level(last_dir_position, next_dir_position,
                                  num_robots - 1);
      last_dir_position = next_dir_position;
    }
    last_position = next_position;
  }
  return sum;
}

std::string part_1(const std::string &filepath) {
  const auto sequences = get_lines_from_file(filepath);

  size_t complexity{};
  for (const auto &sequence : sequences) {
    const size_t length = get_total_length(sequence, 2);
    complexity += (length * get_numeric_part(sequence));
  }

  return std::to_string(complexity);
}

std::string part_2(const std::string &filepath) {
  const auto sequences = get_lines_from_file(filepath);

  size_t complexity{};
  for (const auto &sequence : sequences) {
    const size_t length = get_total_length(sequence, 25);
    complexity += (length * get_numeric_part(sequence));
  }

  return std::to_string(complexity);
}

// clang-format off
} // namespace d21
// clang-format on
