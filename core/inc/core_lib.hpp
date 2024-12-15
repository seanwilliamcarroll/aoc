#pragma once

#include <string>  // for string
#include <utility> // for pair
#include <vector>  // for vector

using Coordinate = long long;

using Position = std::pair<Coordinate, Coordinate>;

using Tile = char;

using Grid = std::vector<std::string>;

void greet_day(int);

Grid get_lines_from_file(const std::string &filepath);

void print_lines(const Grid &);

bool is_in_bounds(const Grid &, const Coordinate row, const Coordinate col);

bool is_in_bounds(const Grid &, const Position &);

void do_assert(bool, const std::string &message);
