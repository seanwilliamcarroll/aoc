#include <array>        // for array
#include <core_lib.hpp> // for Coordinate, Position, Grid, greet_day, is_in...
#include <deque>        // for deque
#include <fstream>      // for basic_ostream, endl, operator<<, basic_istream
#include <iostream>     // for cout, cerr
#include <limits>       // for numeric_limits
#include <stddef.h>     // for size_t
#include <string>       // for char_traits, basic_string, string
#include <utility>      // for pair, make_pair, operator==
#include <vector>       // for vector

using Positions = std::vector<Position>;

constexpr Coordinate GRID_HEIGHT = 71;
constexpr Coordinate GRID_WIDTH = 71;

constexpr Tile OBSTACLE = '#';

constexpr Position START = {0, 0};
constexpr Position END = {GRID_HEIGHT - 1, GRID_WIDTH - 1};

using Movement = Position;

constexpr Coordinate NUM_DIRECTIONS = 4;

constexpr std::array<Movement, NUM_DIRECTIONS> MOVEMENTS = {
    {{-1, 0}, {0, 1}, {1, 0}, {0, -1}}};

constexpr size_t ONE_KILOBYTE = 1024;

Positions get_positions_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  Positions positions;

  Coordinate row = 0;
  Coordinate col = 0;
  char comma;

  while (in_stream >> row >> comma >> col) {
    positions.emplace_back(row, col);
  }
  return positions;
}

void print_positions(const Positions &positions) {
  for (const auto &[row, col] : positions) {
    std::cout << row << " " << col << std::endl;
  }
  std::cout << std::endl;
}

void simulate_n_more_bytes_falling(Grid &grid, const Positions &positions,
                                   const size_t bytes_so_far,
                                   const size_t n_bytes_more) {
  for (size_t byte_index = bytes_so_far;
       byte_index < bytes_so_far + n_bytes_more; ++byte_index) {
    const auto &[row, col] = positions[byte_index];
    grid[row][col] = OBSTACLE;
  }
}

Grid simulate_n_bytes_falling(const Positions &positions,
                              const size_t n_bytes) {
  Grid grid(GRID_HEIGHT, std::string(GRID_WIDTH, '.'));

  simulate_n_more_bytes_falling(grid, positions, 0, n_bytes);

  return grid;
}

size_t find_length_shortest_path(const Grid &grid) {
  using IntermediateResult = std::pair<size_t, Position>;

  std::deque<IntermediateResult> attempts;
  attempts.emplace_back(0, START);

  std::vector<std::vector<size_t>> shortest_so_far(
      GRID_HEIGHT, std::vector(GRID_WIDTH, std::numeric_limits<size_t>::max()));

  while (!attempts.empty()) {
    const auto [path_length, position] = attempts.front();
    attempts.pop_front();
    if (position == END) {
      continue;
    }
    if (path_length > shortest_so_far[END.first][END.second]) {
      continue;
    }
    const auto [row, col] = position;
    for (const auto &[row_incr, col_incr] : MOVEMENTS) {
      const auto new_row = row + row_incr;
      const auto new_col = col + col_incr;
      if (!is_in_bounds(grid, new_row, new_col)) {
        continue;
      }
      if (grid[new_row][new_col] == OBSTACLE) {
        continue;
      }
      if (path_length + 1 >= shortest_so_far[new_row][new_col]) {
        continue;
      }
      shortest_so_far[new_row][new_col] = path_length + 1;
      attempts.emplace_back(path_length + 1, std::make_pair(new_row, new_col));
    }
  }
  return shortest_so_far[END.first][END.second];
}

size_t simulate_and_solve(const Positions &positions) {
  const auto grid = simulate_n_bytes_falling(positions, ONE_KILOBYTE);
  return find_length_shortest_path(grid);
}

size_t simulate_to_failure(const Positions &positions) {
  size_t bytes_so_far = ONE_KILOBYTE + 1;
  auto grid = simulate_n_bytes_falling(positions, bytes_so_far);
  size_t shortest_path = find_length_shortest_path(grid);
  while (bytes_so_far < positions.size() &&
         shortest_path != std::numeric_limits<size_t>::max()) {
    simulate_n_more_bytes_falling(grid, positions, bytes_so_far, 1);
    shortest_path = find_length_shortest_path(grid);
    ++bytes_so_far;
  }

  return bytes_so_far;
}

int main(int argc, char *argv[]) {
  greet_day(18);
  if (argc <= 1) {
    std::cerr << "Must provide filepath!" << std::endl;
    return -1;
  }

  const auto positions = get_positions_from_file(argv[1]);

  size_t shortest_path = simulate_and_solve(positions);

  std::cout << "Part 1: Shortest Path Length: " << shortest_path << std::endl;

  size_t number_to_fall = simulate_to_failure(positions);

  const auto &[row, col] = positions[number_to_fall - 1];

  std::cout << "Part 2: Enclosing Corrupted Position: " << row << "," << col
            << std::endl;

  return 0;
}
