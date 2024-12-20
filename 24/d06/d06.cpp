#include <array>        // for array
#include <core_lib.hpp> // for Grid, Position, Tile, get_lines_from_file
#include <d06.hpp>
#include <set>       // for set, __tree_const_iterator
#include <stddef.h>  // for size_t
#include <stdexcept> // for runtime_error
#include <string>    // for basic_string, string, to_string
#include <utility>   // for pair, make_pair, operator==

namespace d06 {

using PositionSet = std::set<Position>;

using HeadingPositionSet = std::set<std::pair<int, Position>>;

constexpr size_t NUM_DIRECTIONS = 4;

// Listed in order of turning
constexpr std::array<Tile, NUM_DIRECTIONS> GUARD_TILES = {'^', '>', 'v', '<'};

constexpr std::array<Position, NUM_DIRECTIONS> GUARD_MOVEMENTS = {
    {{-1, 0}, {0, 1}, {1, 0}, {0, -1}}};

constexpr Tile VISITED = 'X';
constexpr Tile OBSTACLE = '#';

Position find_guard(const Grid &grid) {
  for (int row_index = 0; row_index < grid.size(); ++row_index) {
    for (int col_index = 0; col_index < grid[row_index].size(); ++col_index) {
      const Tile current_space = grid[row_index][col_index];
      if (current_space == GUARD_TILES[0] || current_space == GUARD_TILES[1] ||
          current_space == GUARD_TILES[2] || current_space == GUARD_TILES[3]) {
        return std::make_pair(row_index, col_index);
      }
    }
  }
  throw std::runtime_error("Should have found the guard!");
}

int get_position_index(const Tile guard_tile) {
  if (guard_tile == GUARD_TILES[0]) {
    return 0;
  } else if (guard_tile == GUARD_TILES[1]) {
    return 1;
  } else if (guard_tile == GUARD_TILES[2]) {
    return 2;
  } else if (guard_tile == GUARD_TILES[3]) {
    return 3;
  }
  throw std::runtime_error("Unknown guard tile!");
}

PositionSet
from_heading_position_set(const HeadingPositionSet &heading_position_set) {
  PositionSet output;
  for (const auto &[_, position] : heading_position_set) {
    output.insert(position);
  }
  return output;
}

std::pair<Tile, HeadingPositionSet>
simulate_guard(const Grid &grid, const Position starting_position) {
  auto [row, col] = starting_position;
  HeadingPositionSet visited;
  auto guard_tile = get_position_index(grid[row][col]);
  while (is_in_bounds(grid, row, col)) {
    const auto current_heading =
        std::make_pair(guard_tile, std::make_pair(row, col));
    if (visited.count(current_heading) > 0) {
      return std::make_pair(OBSTACLE, visited);
    }
    const auto [row_incr, col_incr] = GUARD_MOVEMENTS[guard_tile];
    const auto next_row = row + row_incr;
    const auto next_col = col + col_incr;
    if (!is_in_bounds(grid, next_row, next_col)) {
      visited.insert(std::make_pair(guard_tile, std::make_pair(row, col)));
      return std::make_pair(VISITED, visited);
    }
    const auto next_tile = grid[next_row][next_col];
    if (next_tile == OBSTACLE) {
      guard_tile = (guard_tile + 1) % NUM_DIRECTIONS;
      continue;
    }
    visited.insert(std::make_pair(guard_tile, std::make_pair(row, col)));
    row = next_row;
    col = next_col;
  }
  return std::make_pair(VISITED, visited);
}

PositionSet simulate_guard_get_visited_positions(const Grid &grid) {
  const Position starting_position = find_guard(grid);
  const auto [output_tile, heading_position_set] =
      simulate_guard(grid, starting_position);
  if (output_tile != VISITED) {
    throw std::runtime_error("Expected not to have a cycle!");
  }
  return from_heading_position_set(heading_position_set);
}

size_t count_new_obstacle_candidates(const Grid &original_grid,
                                     const PositionSet &visited_positions) {
  // I assume there is a more efficient way of doing this, but this seemed to
  // work quick enough
  size_t new_obstacle_candidates{};
  Grid scratch_grid(original_grid);
  const Position starting_position = find_guard(original_grid);
  for (const auto &position : visited_positions) {
    if (starting_position == position) {
      continue;
    }
    const auto [row, col] = position;
    const auto old_value = scratch_grid[row][col];
    scratch_grid[row][col] = OBSTACLE;
    const auto [output_tile, _] =
        simulate_guard(scratch_grid, starting_position);
    if (output_tile == OBSTACLE) {
      ++new_obstacle_candidates;
    }
    scratch_grid[row][col] = old_value;
  }
  return new_obstacle_candidates;
}

std::string part_1(const std::string &filepath) {
  const Grid grid = get_lines_from_file(filepath);

  const auto visited_positions = simulate_guard_get_visited_positions(grid);
  auto accumulator = visited_positions.size();

  return std::to_string(accumulator);
}

std::string part_2(const std::string &filepath) {
  const Grid grid = get_lines_from_file(filepath);

  const auto visited_positions = simulate_guard_get_visited_positions(grid);

  auto accumulator = count_new_obstacle_candidates(grid, visited_positions);

  return std::to_string(accumulator);
}

} // namespace d06
