#include <core_lib.hpp>

#include <fstream>
#include <iostream>

Grid get_lines_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  Grid output;

  for (std::string line; std::getline(in_stream, line); /*BLANK*/) {
    output.push_back(line);
  }
  return output;
}

void print_lines(const Grid &lines) {

  for (const auto &line : lines) {
    std::cout << line << std::endl;
  }
  std::cout << std::endl;
}

bool is_in_bounds(const Grid &grid, const Coordinate row,
                  const Coordinate col) {
  return (row >= 0 && row < grid.size() && col >= 0 &&
          col < grid.back().size());
}

bool is_in_bounds(const Grid &grid, const Position &position) {
  const auto [row, col] = position;
  return is_in_bounds(grid, row, col);
}
