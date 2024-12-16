#include <array>        // for array
#include <core_lib.hpp> // for Coordinate, Tile, Grid, get_lines_from_file
#include <deque>        // for deque
#include <iostream>     // for basic_ostream, endl, operator<<, cerr, cout
#include <limits>       // for numeric_limits
#include <map>          // for map
#include <stdexcept>    // for runtime_error
#include <string>       // for char_traits, basic_string
#include <tuple>        // for make_tuple, tuple
#include <utility>      // for pair

// constexpr Tile NOTHING = '.';
constexpr Tile START = 'S';
constexpr Tile WALL = '#';
constexpr Tile END = 'E';

using Score = long long;

using Heading = Coordinate;

using HeadingPosition = std::tuple<Heading, Coordinate, Coordinate>;

// constexpr Heading NORTH = 0;
constexpr Heading EAST = 1;
// constexpr Heading SOUTH = 2;
// constexpr Heading WEST = 3;

using Movement = Position;

constexpr Coordinate NUM_DIRECTIONS = 4;

constexpr std::array<Movement, NUM_DIRECTIONS> HEADING_TO_MOVEMENT = {
    {{-1, 0}, {0, 1}, {1, 0}, {0, -1}}};

HeadingPosition find_start_position(const Grid &grid) {
  for (Coordinate row{}; row < grid.size(); ++row) {
    for (Coordinate col{}; col < grid[row].size(); ++col) {
      if (grid[row][col] == START) {
        return std::make_tuple(EAST, row, col);
      }
    }
  }
  throw std::runtime_error("Expected to find start position!");
}

Score find_shortest_path(const Grid &grid) {
  const auto starting_position = find_start_position(grid);

  using IntermediateResult = std::pair<Score, HeadingPosition>;

  std::deque<IntermediateResult> attempts;
  Score lowest_score = std::numeric_limits<Score>::max();
  std::map<HeadingPosition, Score> position_to_score;

  attempts.emplace_back(0, starting_position);
  while (!attempts.empty()) {
    const auto &attempt = attempts.front();
    const auto [score_so_far, position] = attempt;
    attempts.pop_front();
    const auto [heading, row, col] = position;
    if (grid[row][col] == END) {
      // Found the end
      if (score_so_far < lowest_score) {
        lowest_score = score_so_far;
      }
      continue;
    }
    if ((position_to_score[position] != Score() &&
         position_to_score[position] <= score_so_far) ||
        score_so_far >= lowest_score) {
      // visited and we've seen this score or better
      // Or we've already gone past the best so far
      continue;
    }

    const auto [row_incr, col_incr] = HEADING_TO_MOVEMENT[heading];
    const auto new_row = row + row_incr;
    const auto new_col = col + col_incr;

    // Can either take a step forward for cost of 1
    if (grid[new_row][new_col] != WALL) {
      attempts.emplace_back(score_so_far + 1,
                            std::make_tuple(heading, new_row, new_col));
    }

    // Or rotate for a cost of 1000
    {
      const auto new_heading = (heading + 1) % NUM_DIRECTIONS;
      attempts.emplace_back(score_so_far + 1000,
                            std::make_tuple(new_heading, row, col));
    }
    {
      const auto new_heading = (NUM_DIRECTIONS + heading - 1) % NUM_DIRECTIONS;
      attempts.emplace_back(score_so_far + 1000,
                            std::make_tuple(new_heading, row, col));
    }

    position_to_score[position] = score_so_far;
  }

  return lowest_score;
}

int main(int argc, char *argv[]) {
  greet_day(15);
  if (argc <= 1) {
    std::cerr << "Must provide filepath!" << std::endl;
    return -1;
  }

  const auto grid = get_lines_from_file(argv[1]);

  // print_lines(grid);

  Score accumulator = find_shortest_path(grid);

  std::cout << "Part 1: Score: " << accumulator << std::endl;

  return 0;
}
