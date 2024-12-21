#include <algorithm>    // for reverse
#include <array>        // for array
#include <cmath>        // for abs
#include <core_lib.hpp> // for Position, Coordinate, Grid, Matrix2D, Tile
#include <d20.hpp>
#include <deque>     // for deque
#include <limits>    // for numeric_limits
#include <map>       // for map, __map_iterator
#include <stddef.h>  // for size_t
#include <stdexcept> // for runtime_error
#include <stdlib.h>  // for abs
#include <string>    // for string, basic_string, to_string, operator+
#include <tuple>     // for tuple
#include <utility>   // for pair, make_pair, operator==
#include <vector>    // for vector

// clang-format off
namespace d20 {
// clang-format on

constexpr Tile START = 'S';
constexpr Tile END = 'E';
constexpr Tile WALL = '#';

using Positions = std::vector<Position>;

using Movement = Position;

constexpr Coordinate NUM_DIRECTIONS = 4;

constexpr std::array<Movement, NUM_DIRECTIONS> MOVEMENTS = {
    {{-1, 0}, {0, 1}, {1, 0}, {0, -1}}};

using PathLocations = Matrix2D<Position>;
using PathLengths = Matrix2D<size_t>;

// Where did the cheat begin, where did it end, how much savings?
using Cheat = std::tuple<Position, Position, size_t>;

Position find_tile(const Grid &grid, const Tile tile) {
  // return first position where we find this tile
  for (Coordinate row{}; row < grid.size(); ++row) {
    for (Coordinate col{}; col < grid[row].size(); ++col) {
      if (grid[row][col] == tile) {
        return std::make_pair(row, col);
      }
    }
  }
  throw std::runtime_error(std::string("Expected to find tile: ") +
                           std::to_string(tile));
}

std::pair<PathLocations, PathLengths> find_shortest_paths(const Grid &grid) {
  using IntermediateResult = std::tuple<size_t, Position>;

  const auto start = find_tile(grid, START);
  const auto end = find_tile(grid, END);

  std::deque<IntermediateResult> attempts;
  attempts.emplace_back(0, start);

  PathLengths shortest_so_far(
      grid.size(), std::vector<size_t>(grid.back().size(),
                                       std::numeric_limits<size_t>::max()));

  PathLocations last_position(
      grid.size(),
      std::vector<Position>(grid.back().size(), std::make_pair(-1, -1)));

  while (!attempts.empty()) {
    const auto [path_length, position] = attempts.front();
    attempts.pop_front();
    if (position == end) {
      continue;
    }
    if (path_length > shortest_so_far[end.first][end.second]) {
      continue;
    }
    const auto [row, col] = position;
    for (const auto &[row_incr, col_incr] : MOVEMENTS) {
      const auto new_row = row + row_incr;
      const auto new_col = col + col_incr;
      if (!is_in_bounds(grid, new_row, new_col)) {
        continue;
      }
      if (grid[new_row][new_col] == WALL) {
        continue;
      }
      if (path_length + 1 >= shortest_so_far[new_row][new_col]) {
        continue;
      }
      shortest_so_far[new_row][new_col] = path_length + 1;
      last_position[new_row][new_col] = position;
      attempts.emplace_back(path_length + 1, std::make_pair(new_row, new_col));
    }
  }

  return std::make_pair(last_position, shortest_so_far);
}

Positions find_shortest_path(const Grid &grid,
                             const PathLocations &last_position) {
  Positions shortest_path;

  const auto start = find_tile(grid, START);
  const auto end = find_tile(grid, END);

  auto position = end;
  while (position != start) {
    const auto [row, col] = position;
    const auto next_position = last_position[row][col];
    shortest_path.push_back(position);
    position = next_position;
  }

  std::reverse(shortest_path.begin(), shortest_path.end());
  return shortest_path;
}

std::vector<Cheat> find_all_cheats(const Grid &grid,
                                   const Coordinate total_distance) {

  std::vector<Cheat> cheats{};

  const auto start = find_tile(grid, START);
  const auto end = find_tile(grid, END);

  const auto [last_positions, shortest_so_far] = find_shortest_paths(grid);

  const auto shortest_path = find_shortest_path(grid, last_positions);

  std::map<Position, size_t> distance_so_far;
  distance_so_far[start] = 0;
  size_t distance = 1;
  for (const auto &position : shortest_path) {
    distance_so_far[position] = distance;
    ++distance;
  }

  // find all possible cheating positions
  std::map<Position, Coordinate> cheating_increments;
  for (Coordinate row_incr = (-total_distance); row_incr <= total_distance;
       ++row_incr) {
    for (Coordinate col_incr = (-total_distance); col_incr <= total_distance;
         ++col_incr) {
      const auto distance = std::abs(row_incr) + std::abs(col_incr);
      if (distance > 0 && distance <= total_distance) {
        const auto position = std::make_pair(row_incr, col_incr);
        cheating_increments[position] = distance;
      }
    }
  }

  // At each point on the path, try to cheat, but only if it brings us to a
  // point further away than we are now
  size_t position_index = 0;
  Position position = start;

  while (position != end) {
    // Try cheat
    const auto [row, col] = position;
    for (const auto &[incr, distance] : cheating_increments) {
      const auto [row_incr, col_incr] = incr;
      const auto new_row = row + row_incr;
      const auto new_col = col + col_incr;
      if (!is_in_bounds(grid, new_row, new_col)) {
        continue;
      }
      Position cheat_end = std::make_pair(new_row, new_col);
      if (distance_so_far[cheat_end] > (distance_so_far[position] + distance)) {
        cheats.emplace_back(position, cheat_end,
                            distance_so_far[cheat_end] -
                                (distance_so_far[position] + distance));
      }
    }
    position = shortest_path[position_index];
    position_index++;
  }

  return cheats;
}

size_t number_of_cheats_at_least_100_savings(const Grid &grid,
                                             const Coordinate total_distance) {

  const auto [last_position, shortest_so_far] = find_shortest_paths(grid);

  const auto shortest_path = find_shortest_path(grid, last_position);

  const auto cheats = find_all_cheats(grid, total_distance);

  std::map<size_t, size_t> savings_counts;

  for (const auto &[cheat_begin, cheat_end, savings] : cheats) {
    ++savings_counts[savings];
  }

  size_t at_least_100{};
  for (const auto &[savings, count] : savings_counts) {
    if (savings >= 100) {
      at_least_100 += count;
    }
  }

  return at_least_100;
}

std::string part_1(const std::string &filepath) {

  const auto grid = get_lines_from_file(filepath);

  const auto at_least_100 = number_of_cheats_at_least_100_savings(grid, 2);

  return std::to_string(at_least_100);
}

std::string part_2(const std::string &filepath) {

  const auto grid = get_lines_from_file(filepath);

  const auto at_least_100 = number_of_cheats_at_least_100_savings(grid, 20);

  return std::to_string(at_least_100);
}

// clang-format off
} // namespace d20
// clang-format on
