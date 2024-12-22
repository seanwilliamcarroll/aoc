#include <core_lib.hpp> // for Position, Tile, get_lines_from_file
#include <d21.hpp>
#include <map>      // for map
#include <stddef.h> // for size_t
#include <string>   // for basic_string, allocator, operator+, char_traits
#include <utility>  // for pair, make_pair, operator==
#include <vector>   // for vector

// clang-format off
namespace d21 {
// clang-format on

using Sequence = std::string;
using Sequences = std::vector<Sequence>;

constexpr Tile INVALID = 99;

// constexpr size_t NUMBER_PAD_ROWS = 4;
// constexpr size_t NUMBER_PAD_COLS = 3;
// constexpr std::array<std::array<Tile, NUMBER_PAD_COLS>, NUMBER_PAD_ROWS>
//     NUMBER_PAD = {
//   {{'7', '8', '9'}, {'4', '5', '6'}, {'1', '2', '3'}, {INVALID, '0', 'A'}}};

const std::map<Tile, Position> NUMBER_PAD_POSITIONS = {
    {'7', {0, 0}}, {'8', {0, 1}},     {'9', {0, 2}}, {'4', {1, 0}},
    {'5', {1, 1}}, {'6', {1, 2}},     {'1', {2, 0}}, {'2', {2, 1}},
    {'3', {2, 2}}, {INVALID, {3, 0}}, {'0', {3, 1}}, {'A', {3, 2}}};

// constexpr size_t DIRECTION_PAD_ROWS = 2;
// constexpr size_t DIRECTION_PAD_COLS = 3;
// constexpr std::array<std::array<Tile, DIRECTION_PAD_COLS>,
// DIRECTION_PAD_ROWS> DIRECTION_PAD = {{{INVALID, '^', 'A'}, {'<', 'v', '>'}}};

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

std::pair<Sequence, Position>
get_sequence(const Position &initial_position, const Tile target_tile,
             const std::map<Tile, Position> &pad_positions) {
  const Position target_position = pad_positions.at(target_tile);
  const auto &[target_row, target_col] = target_position;
  const auto &[initial_row, initial_col] = initial_position;

  const auto invalid_position = pad_positions.at(INVALID);

  // Need to try both row and column as priority to see which gives us the
  // faster solution??

  // Do we only need to move in one direction
  if (target_row == initial_row && target_col == initial_col) {
    return std::make_pair(ENTER_SEQUENCE, target_position);
  } else if (target_row == initial_row) {
    if (initial_col < target_col) {
      return std::make_pair(Sequence(target_col - initial_col, '>') +
                                ENTER_SEQUENCE,
                            target_position);
    } else {
      return std::make_pair(Sequence(initial_col - target_col, '<') +
                                ENTER_SEQUENCE,
                            target_position);
    }
  } else if (target_col == initial_col) {
    if (initial_row < target_row) {
      return std::make_pair(Sequence(target_row - initial_row, 'v') +
                                ENTER_SEQUENCE,
                            target_position);
    } else {
      return std::make_pair(Sequence(initial_row - target_row, '^') +
                                ENTER_SEQUENCE,
                            target_position);
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
    return std::make_pair(horizontal + vertical + ENTER_SEQUENCE,
                          target_position);
  } else if (std::make_pair(target_row, initial_col) == invalid_position) {
    // Going horizontal first will hit invalid
    return std::make_pair(vertical + horizontal + ENTER_SEQUENCE,
                          target_position);
  }

  if (initial_row > target_row && initial_col > target_col) {
    return std::make_pair(vertical + horizontal + ENTER_SEQUENCE,
                          target_position);
  } else if (initial_row > target_row && initial_col < target_col) {
    return std::make_pair(horizontal + vertical + ENTER_SEQUENCE,
                          target_position);
  } else if (initial_row < target_row && initial_col > target_col) {
    return std::make_pair(vertical + horizontal + ENTER_SEQUENCE,
                          target_position);
  } else { // if (initial_row < target_row && initial_col < target_col) {
    return std::make_pair(horizontal + vertical + ENTER_SEQUENCE,
                          target_position);
  }
}

Sequence get_full_sequence(const Sequence &sequence,
                           const std::map<Tile, Position> &pad_positions) {
  auto position = pad_positions.at(ENTER_KEY);
  Sequence output;
  for (const auto tile : sequence) {
    const auto [sub_sequence, next_position] =
        get_sequence(position, tile, pad_positions);
    output += sub_sequence;
    position = next_position;
  }
  return output;
}

Sequence get_complete_sequence(const Sequence &sequence,
                               const size_t number_robots) {
  const Sequence numeric_sequence =
      get_full_sequence(sequence, NUMBER_PAD_POSITIONS);
  // std::cout << numeric_sequence << std::endl;

  Sequence robot_sequence = numeric_sequence;
  for (size_t index{}; index < number_robots - 1; ++index) {
    robot_sequence = get_full_sequence(robot_sequence, DIRECTION_PAD_POSITIONS);
  }
  // std::cout << robot_sequence << std::endl;

  const Sequence human_sequence =
      get_full_sequence(robot_sequence, DIRECTION_PAD_POSITIONS);
  // std::cout << human_sequence << std::endl;

  return human_sequence;
}

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

std::string part_1(const std::string &filepath) {

  const auto sequences = get_lines_from_file(filepath);

  // print_lines(sequences);

  // const Position initial_position = NUMBER_PAD_POSITIONS.at('A');

  // const Position target_position = NUMBER_PAD_POSITIONS.at('7');

  size_t complexity{};
  for (const auto &sequence : sequences) {
    const Sequence output_sequence = get_complete_sequence(sequence, 2);
    // std::cout << sequence << std::endl;
    // std::cout << output_sequence << std::endl;
    // std::cout << "Length: " << output_sequence.size() << std::endl;
    // std::cout << "Numeric Part: " << get_numeric_part(sequence) << std::endl;
    complexity += (output_sequence.size() * get_numeric_part(sequence));
  }

  return std::to_string(complexity);
}

std::string part_2(const std::string &filepath) { return std::string(); }

// clang-format off
} // namespace d21
// clang-format on
