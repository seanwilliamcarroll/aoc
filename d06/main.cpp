#include <array>     // for array
#include <fstream>   // for basic_ostream, operator<<, endl, basic_istream
#include <iostream>  // for cout, cerr
#include <set>       // for set
#include <stddef.h>  // for size_t
#include <stdexcept> // for runtime_error
#include <string>    // for char_traits, basic_string, string, operator+
#include <utility>   // for pair, make_pair
#include <vector>    // for vector

using Grid = std::vector<std::string>;

using Coordinates = std::pair<int, int>;

constexpr char OBSTACLE = '#';

constexpr size_t NUM_DIRECTIONS = 4;

// Listed in order of turning
constexpr std::array<char, NUM_DIRECTIONS> GUARD_TILES = {'^', '>', 'v', '<'};

constexpr std::array<Coordinates, NUM_DIRECTIONS> GUARD_MOVEMENTS = {
    {{-1, 0}, {0, 1}, {1, 0}, {0, -1}}};

constexpr char VISITED = 'X';

Coordinates get_guard_movement(const char guard_tile) {
  for (size_t pos = 0; pos < GUARD_TILES.size(); ++pos) {
    if (GUARD_TILES[pos] == guard_tile) {
      return GUARD_MOVEMENTS[pos];
    }
  }
  throw std::runtime_error(std::string("Unexpected guard position: ") +
                           guard_tile);
}

Grid get_grid_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  Grid grid;
  for (std::string line; std::getline(in_stream, line); /*BLANK*/) {
    grid.push_back(line);
  }

  return grid;
}

void print_grid(const Grid &grid) {
  for (const auto &row : grid) {
    std::cout << row << std::endl;
  }
  std::cout << std::endl;
}

Coordinates find_guard(const Grid &grid) {
  for (int row_index = 0; row_index < grid.size(); ++row_index) {
    for (int col_index = 0; col_index < grid[row_index].size(); ++col_index) {
      const char current_space = grid[row_index][col_index];
      if (current_space == GUARD_TILES[0] || current_space == GUARD_TILES[1] ||
          current_space == GUARD_TILES[2] || current_space == GUARD_TILES[3]) {
        return std::make_pair(row_index, col_index);
      }
    }
  }
  throw std::runtime_error("Should have found the guard!");
  return std::make_pair(0, 0);
}

bool in_bounds(const Grid &grid, const int row_index, const int col_index) {
  return (row_index >= 0 && row_index <= grid.size() && col_index >= 0 &&
          col_index <= grid[row_index].size());
}

char rotate_guard(const char guard_tile) {
  size_t pos{};
  while (pos < NUM_DIRECTIONS && GUARD_TILES[++pos] != guard_tile)
    ;
  return GUARD_TILES[((pos + 1) % NUM_DIRECTIONS)];
}

char simulate_guard(Grid &grid) {
  Coordinates starting_position = find_guard(grid);
  int row_index(starting_position.first);
  int col_index(starting_position.second);
  std::set<std::pair<char, Coordinates>> visited;
  while (in_bounds(grid, row_index, col_index)) {
    const auto guard_tile = grid[row_index][col_index];
    const auto current_heading =
        std::make_pair(guard_tile, std::make_pair(row_index, col_index));
    if (visited.count(current_heading) > 0) {
      return OBSTACLE;
    }
    const auto next_movement = get_guard_movement(guard_tile);
    const auto next_row_index = row_index + next_movement.first;
    const auto next_col_index = col_index + next_movement.second;
    if (!in_bounds(grid, next_row_index, next_col_index)) {
      grid[row_index][col_index] = VISITED;
      return VISITED;
    }
    const auto next_tile = grid[next_row_index][next_col_index];
    if (next_tile == OBSTACLE) {
      grid[row_index][col_index] = rotate_guard(guard_tile);
      continue;
    }
    grid[row_index][col_index] = VISITED;
    visited.insert(
        std::make_pair(guard_tile, std::make_pair(row_index, col_index)));
    grid[next_row_index][next_col_index] = guard_tile;
    row_index = next_row_index;
    col_index = next_col_index;
  }
  return VISITED;
}

Grid simulate_guard_const(const Grid &input_grid) {
  Grid grid(input_grid);
  const auto output_tile = simulate_guard(grid);
  if (output_tile != VISITED) {
    throw std::runtime_error("Expected not to have a cycle!");
  }
  return grid;
}

size_t count_visited(const Grid &grid) {
  size_t total_visited{};
  for (int row_index = 0; row_index < grid.size(); ++row_index) {
    for (int col_index = 0; col_index < grid[row_index].size(); ++col_index) {
      const char current_space = grid[row_index][col_index];
      if (current_space == VISITED) {
        ++total_visited;
      }
    }
  }
  return total_visited;
}

bool contains_guard(const Grid &grid, const int row_index,
                    const int col_index) {
  const auto tile = grid[row_index][col_index];
  return (tile == GUARD_TILES[0] || tile == GUARD_TILES[1] ||
          tile == GUARD_TILES[2] || tile == GUARD_TILES[3]);
}

size_t count_new_obstacle_candidates(const Grid &original_grid,
                                     const Grid &visited_grid) {
  // I assume there is a more efficient way of doing this, but this seemed to
  // work quick enough
  size_t new_obstacle_candidates{};
  Grid scratch_grid(original_grid);
  for (int row_index = 0; row_index < visited_grid.size(); ++row_index) {
    for (int col_index = 0; col_index < visited_grid[row_index].size();
         ++col_index) {
      const char current_space = visited_grid[row_index][col_index];
      if (!contains_guard(original_grid, row_index, col_index) &&
          current_space == VISITED) {
        scratch_grid = original_grid;
        scratch_grid[row_index][col_index] = OBSTACLE;
        const auto outcome = simulate_guard(scratch_grid);
        if (outcome == OBSTACLE) {
          ++new_obstacle_candidates;
        }
      }
    }
  }
  return new_obstacle_candidates;
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    std::cerr << "Must provide filepath!" << std::endl;
    return -1;
  }

  const auto grid = get_grid_from_file(argv[1]);

  const auto visited_grid = simulate_guard_const(grid);

  int accumulator = count_visited(visited_grid);

  std::cout << "Part 1: Visited " << accumulator << " tiles" << std::endl;

  accumulator = count_new_obstacle_candidates(grid, visited_grid);

  std::cout << "Part 2: New Obstacle Candidates " << accumulator << " tiles"
            << std::endl;

  return 0;
}
