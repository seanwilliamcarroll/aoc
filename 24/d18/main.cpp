#include <array>        // for array
#include <core_lib.hpp> // for Position, Coordinate, Grid, Matrix2D, greet_day
#include <deque>        // for deque
#include <fstream>      // for basic_ostream, endl, operator<<, basic_istream
#include <iostream>     // for cout, cerr
#include <limits>       // for numeric_limits
#include <set>          // for set
#include <stddef.h>     // for size_t
#include <string>       // for char_traits, basic_string, string
#include <tuple>        // for tuple
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

using PathLocations = Matrix2D<Position>;
using PathLengths = Matrix2D<size_t>;

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

std::pair<PathLocations, PathLengths> find_shortest_paths(const Grid &grid) {
  using IntermediateResult = std::tuple<size_t, Position>;

  std::deque<IntermediateResult> attempts;
  attempts.emplace_back(0, START);

  PathLengths shortest_so_far(
      GRID_HEIGHT,
      std::vector<size_t>(GRID_WIDTH, std::numeric_limits<size_t>::max()));

  PathLocations last_position(
      GRID_HEIGHT, std::vector<Position>(GRID_WIDTH, std::make_pair(-1, -1)));

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
      last_position[new_row][new_col] = position;
      attempts.emplace_back(path_length + 1, std::make_pair(new_row, new_col));
    }
  }

  return std::make_pair(last_position, shortest_so_far);
}

size_t find_length_shortest_path(const Grid &grid) {
  const auto [_, shortest_paths] = find_shortest_paths(grid);
  return shortest_paths[END.first][END.second];
}

size_t find_length_shortest_path(const PathLengths &shortest_paths) {
  return shortest_paths[END.first][END.second];
}

size_t simulate_and_solve(const Positions &positions) {
  const auto grid = simulate_n_bytes_falling(positions, ONE_KILOBYTE);
  return find_length_shortest_path(grid);
}

std::set<Position> find_shortest_path(const PathLocations &last_position) {
  std::set<Position> shortest_path;
  shortest_path.insert(END);
  shortest_path.insert(START);
  auto position = END;
  while (position != START) {
    const auto [row, col] = position;
    const auto next_position = last_position[row][col];
    shortest_path.insert(position);
    position = next_position;
  }
  return shortest_path;
}

size_t simulate_to_failure(const Positions &positions) {
  size_t bytes_so_far = ONE_KILOBYTE + 1;
  auto grid = simulate_n_bytes_falling(positions, bytes_so_far);
  auto [last_position, shortest_paths] = find_shortest_paths(grid);
  size_t shortest_path_length = find_length_shortest_path(shortest_paths);

  std::set<Position> shortest_path = find_shortest_path(last_position);

  while (bytes_so_far < positions.size() &&
         shortest_path_length != std::numeric_limits<size_t>::max()) {
    simulate_n_more_bytes_falling(grid, positions, bytes_so_far, 1);
    // Only do a new search when we have positions that fall on our current
    // shortest path
    if (shortest_path.count(positions[bytes_so_far]) > 0) {
      auto path_pair = find_shortest_paths(grid);
      last_position = path_pair.first;
      shortest_paths = path_pair.second;
      shortest_path_length = find_length_shortest_path(shortest_paths);
      if (shortest_path_length != std::numeric_limits<size_t>::max()) {
        shortest_path = find_shortest_path(last_position);
      }
    }
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
