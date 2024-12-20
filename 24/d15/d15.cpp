#include <d15.hpp>
#include <core_lib.hpp>  // for Coordinate, Grid, Tile, Position
#include <fstream>       // for basic_istream, basic_ifstream, getline, ifst...
#include <map>           // for map
#include <set>           // for set, __tree_const_iterator
#include <stdexcept>     // for runtime_error
#include <string>        // for basic_string, char_traits, string, to_string
#include <utility>       // for swap, pair, make_pair

namespace d15 {

using Movement = Position;

using Instructions = std::string;

using Instruction = char;

constexpr Instruction NORTH = '^';
constexpr Instruction EAST = '>';
constexpr Instruction SOUTH = 'v';
constexpr Instruction WEST = '<';

const std::map<Instruction, Movement> INSTRUCTION_TO_MOVEMENT = {
    {NORTH, {-1, 0}}, {EAST, {0, 1}}, {SOUTH, {1, 0}}, {WEST, {0, -1}}};

constexpr Tile NOTHING = '.';
constexpr Tile ROBOT = '@';
constexpr Tile WALL = '#';
constexpr Tile BOX = 'O';
constexpr Tile BOX_LEFT = '[';
constexpr Tile BOX_RIGHT = ']';

std::pair<Grid, Instructions>
get_grid_and_instructions_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  Grid grid;
  Instructions instructions;
  bool finished_grid = false;
  for (std::string line; std::getline(in_stream, line); /*BLANK*/) {
    if (line.size() == 0) {
      finished_grid = true;
    }
    if (!finished_grid) {
      grid.push_back(line);
    } else {
      instructions += line;
    }
  }

  return std::make_pair(grid, instructions);
}

Position get_initial_robot_position(const Grid &grid) {
  for (Coordinate row{}; row < grid.size(); ++row) {
    for (Coordinate col{}; col < grid[row].size(); ++col) {
      if (grid[row][col] == ROBOT) {
        return std::make_pair(row, col);
      }
    }
  }
  throw std::runtime_error("Could not find robot!");
}

bool try_do_move(Grid &grid, const Coordinate row, const Coordinate col,
                 const Coordinate row_incr, const Coordinate col_incr) {
  // Given our current position, see if we can move by row_incr, col_incr
  const Coordinate new_row = row + row_incr;
  const Coordinate new_col = col + col_incr;

  if (grid[new_row][new_col] == WALL) {
    return false;
  }
  if (grid[new_row][new_col] != BOX) {
    std::swap(grid[row][col], grid[new_row][new_col]);
    return true;
  }
  // We're trying to move into a box
  Coordinate row_forward = new_row + row_incr;
  Coordinate col_forward = new_col + col_incr;
  while (grid[row_forward][col_forward] == BOX) {
    row_forward += row_incr;
    col_forward += col_incr;
  }
  if (grid[row_forward][col_forward] == WALL) {
    return false;
  }
  // Move BOX first
  std::swap(grid[new_row][new_col], grid[row_forward][col_forward]);
  // Finally can move ROBOT
  std::swap(grid[row][col], grid[new_row][new_col]);
  return true;
}

bool try_do_wide_move(Grid &grid, const Coordinate row, const Coordinate col,
                      const Coordinate row_incr, const Coordinate col_incr) {
  // Given our current position, see if we can move by row_incr, col_incr
  const Coordinate new_row = row + row_incr;
  const Coordinate new_col = col + col_incr;

  if (grid[new_row][new_col] == WALL) {
    return false;
  }
  if (grid[new_row][new_col] != BOX_LEFT &&
      grid[new_row][new_col] != BOX_RIGHT) {
    std::swap(grid[row][col], grid[new_row][new_col]);
    return true;
  }
  // We're trying to move into a box

  // Moving EAST or WEST is easier than NORTH or SOUTH
  if (row_incr == 0) {
    // EAST or WEST
    Coordinate col_forward = new_col + col_incr;
    while (grid[row][col_forward] == BOX_LEFT ||
           grid[row][col_forward] == BOX_RIGHT) {
      col_forward += col_incr;
    }
    if (grid[row][col_forward] == WALL) {
      return false;
    }
    // Move BOXs first
    while (col_forward != new_col) {
      std::swap(grid[row][col_forward - col_incr], grid[row][col_forward]);
      col_forward -= col_incr;
    }
    // Finally can move ROBOT
    std::swap(grid[row][col], grid[row][new_col]);
    return true;
  }
  // NORTH or SOUTH
  std::map<Coordinate, std::set<Coordinate>> cols_to_check;
  cols_to_check[new_row].insert(col);

  if (grid[new_row][col] == BOX_LEFT) {
    cols_to_check[new_row].insert(col + 1);
  } else {
    cols_to_check[new_row].insert(col - 1);
  }
  Coordinate row_forward = new_row + row_incr;
  bool look_at_next_row = true;
  while (look_at_next_row) {
    for (const auto col_to_check : cols_to_check[row_forward - row_incr]) {
      if (grid[row_forward][col_to_check] == WALL) {
        return false;
      } else if (grid[row_forward][col_to_check] == BOX_LEFT) {
        cols_to_check[row_forward].insert(col_to_check);
        cols_to_check[row_forward].insert(col_to_check + 1);
      } else if (grid[row_forward][col_to_check] == BOX_RIGHT) {
        cols_to_check[row_forward].insert(col_to_check);
        cols_to_check[row_forward].insert(col_to_check - 1);
      }
    }
    // If we hit a row of all nothing, stop moving forward
    look_at_next_row = !cols_to_check[row_forward].empty();
    if (look_at_next_row) {
      row_forward += row_incr;
    }
  }
  // row_forward now has a row of NOTHING, work backwards and swap forward
  while (row_forward != new_row) {
    for (const auto col_to_move : cols_to_check[row_forward - row_incr]) {
      std::swap(grid[row_forward][col_to_move],
                grid[row_forward - row_incr][col_to_move]);
    }
    row_forward -= row_incr;
  }
  // move ROBOT
  std::swap(grid[row][col], grid[new_row][col]);
  return true;
}

void simulate_robot(Grid &grid, const Instructions &instructions,
                    const bool is_wide) {
  const Position initial_position = get_initial_robot_position(grid);

  auto [row, col] = initial_position;

  for (const auto instruction : instructions) {
    const auto [row_incr, col_incr] = INSTRUCTION_TO_MOVEMENT.at(instruction);
    if ((!is_wide && try_do_move(grid, row, col, row_incr, col_incr)) ||
        (is_wide && try_do_wide_move(grid, row, col, row_incr, col_incr))) {
      row += row_incr;
      col += col_incr;
    }
  }
}

Coordinate get_gps_of_box(const Coordinate row, const Coordinate col) {
  return (row * 100) + col;
}

Coordinate get_gps_total(const Grid &grid) {
  Coordinate total{};
  for (Coordinate row{}; row < grid.size(); ++row) {
    for (Coordinate col{}; col < grid[row].size(); ++col) {
      if (grid[row][col] == BOX || grid[row][col] == BOX_LEFT) {
        total += get_gps_of_box(row, col);
      }
    }
  }
  return total;
}

Coordinate simulate_and_get_gps_total(const Grid &input_grid,
                                      const Instructions &instructions,
                                      const bool is_wide) {
  Grid grid(input_grid);

  simulate_robot(grid, instructions, is_wide);

  return get_gps_total(grid);
}

Grid widen_grid(const Grid &input_grid) {
  Grid grid;
  for (const auto &row : input_grid) {
    grid.emplace_back();
    for (const auto tile : row) {
      if (tile == ROBOT) {
        grid.back().push_back(ROBOT);
        grid.back().push_back(NOTHING);
      } else if (tile == NOTHING) {
        grid.back().push_back(NOTHING);
        grid.back().push_back(NOTHING);
      } else if (tile == BOX) {
        grid.back().push_back(BOX_LEFT);
        grid.back().push_back(BOX_RIGHT);
      } else if (tile == WALL) {
        grid.back().push_back(WALL);
        grid.back().push_back(WALL);
      } else {
        throw std::runtime_error("Unexpected tile!");
      }
    }
  }

  return grid;
}

std::string part_1(const std::string &filepath) {

  const auto [grid, instructions] =
      get_grid_and_instructions_from_file(filepath);

  bool is_wide = false;

  Coordinate accumulator =
      simulate_and_get_gps_total(grid, instructions, is_wide);


  return std::to_string(accumulator);

}

std::string part_2(const std::string &filepath) {

  const auto [grid, instructions] =
      get_grid_and_instructions_from_file(filepath);

  bool is_wide = true;

  const auto wide_grid = widen_grid(grid);

  Coordinate accumulator =
      simulate_and_get_gps_total(wide_grid, instructions, is_wide);

  return std::to_string(accumulator);

}

} // namespace d15
