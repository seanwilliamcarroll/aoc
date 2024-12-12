#include <algorithm> // for sort
#include <array>     // for array
#include <deque>     // for deque
#include <fstream>   // for basic_ostream, endl, operator<<, basic_istream
#include <iostream>  // for cout, cerr
#include <map>       // for map, __map_iterator
#include <set>       // for set, __tree_const_iterator
#include <stddef.h>  // for size_t
#include <string>    // for char_traits, basic_string, string
#include <utility>   // for pair, make_pair
#include <vector>    // for vector

using Garden = std::vector<std::string>;

using Unit = long long;

using AreaPerimeter = std::pair<Unit, Unit>;

using Tile = char;

using Coordinate = long long;

using Position = std::pair<Coordinate, Coordinate>;

using Measurement = std::pair<Position, AreaPerimeter>;

constexpr std::array<Position, 4> MOVEMENTS = {
    {{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};

constexpr Tile SEEN = '_';

Garden get_garden_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  Garden garden;
  for (std::string line; std::getline(in_stream, line); /*BLANK*/) {
    garden.push_back(line);
  }

  return garden;
}

void print_garden(const Garden &garden) {
  for (const auto &row : garden) {
    std::cout << row << std::endl;
  }
  std::cout << std::endl;
}

bool is_in_bounds(const Garden &garden, const Position &position) {
  const auto [row, col] = position;
  return (row >= 0 && row < garden.size() && col >= 0 &&
          col < garden.back().size());
}

bool is_same_area(const Garden &garden, const Tile region_type,
                  const Position &position) {
  const auto [row, col] = position;
  return is_in_bounds(garden, position) && region_type == garden[row][col];
}

Unit count_sides(
    const Garden &garden, const Position &region,
    const std::set<std::pair<Coordinate, Position>> &sides_touching_perimeter) {
  Unit num_sides = 0;

  // Need to keep track of continuous sides

  // Keep track of all tiles on the perimeter

  // merge those that touch into single sides until no more merging?

  // index 0, 2 move columns
  // index 1, 3 move rows
  // Map pair of (movement_index, coordinate not affected by movement (e.g. col
  // if moves columns)) to other coordinate
  std::map<std::pair<Coordinate, Coordinate>, std::vector<Coordinate>>
      side_segments;

  for (const auto &[movement_index, position] : sides_touching_perimeter) {
    const auto [row, col] = position;

    if (movement_index == 0 || movement_index == 2) {
      const std::pair<Coordinate, Coordinate> unique_side =
          std::make_pair(movement_index, col);
      side_segments[unique_side].push_back(row);
    } else {
      const std::pair<Coordinate, Coordinate> unique_side =
          std::make_pair(movement_index, row);
      side_segments[unique_side].push_back(col);
    }
  }

  for (auto &[unique_side, segments] : side_segments) {
    std::sort(segments.begin(), segments.end());
    ++num_sides;
    for (size_t index = 0; index < segments.size(); ++index) {
      if (index == 0) {
        continue;
      }
      if (segments[index - 1] + 1 != segments[index]) {
        ++num_sides;
      }
    }
  }

  return num_sides;
}

Unit get_region_price(Garden &garden, const Position region,
                      const bool is_part_2 = false) {
  auto [start_row, start_col] = region;

  const Tile region_type = garden[start_row][start_col];

  Unit area = 0;
  Unit perimeter = 0;
  std::deque<Position> attempts{{region}};

  std::set<Position> seen{};

  std::set<std::pair<Coordinate, Position>> sides_touching_perimeter;

  while (!attempts.empty()) {
    auto attempt = attempts.front();
    auto [row, col] = attempt;
    attempts.pop_front();
    if (seen.count(attempt) > 0) {
      continue;
    }
    seen.insert(attempt);

    for (Coordinate movement_index = 0; movement_index < MOVEMENTS.size();
         ++movement_index) {
      const auto &movement = MOVEMENTS[movement_index];
      const auto [row_incr, col_incr] = movement;

      const auto new_row = row + row_incr;
      const auto new_col = col + col_incr;
      const auto new_pos = std::make_pair(new_row, new_col);

      if (!is_in_bounds(garden, new_pos)) {
        perimeter += 1;
        if (is_part_2) {
          sides_touching_perimeter.insert(
              std::make_pair(movement_index, attempt));
        }
        continue;
      }
      if (garden[new_row][new_col] != region_type) {
        perimeter += 1;
        if (is_part_2) {
          sides_touching_perimeter.insert(
              std::make_pair(movement_index, attempt));
        }
        continue;
      }
      if (seen.count(new_pos) > 0) {
        continue;
      }
      attempts.push_back(new_pos);
    }
  }

  area = seen.size();

  if (is_part_2) {
    if (area <= 2) {
      perimeter = 4;
    } else {
      perimeter = count_sides(garden, region, sides_touching_perimeter);
    }
  }

  for (const auto &position : seen) {
    auto [row, col] = position;
    garden[row][col] = SEEN;
  }

  return area * perimeter;
}

Unit get_total_price(const Garden &input_garden, const bool is_part_2 = false) {
  Unit accumulator{};

  Garden garden(input_garden);

  for (Coordinate row{}; row < garden.size(); ++row) {
    for (Coordinate col{}; col < garden[row].size(); ++col) {
      if (garden[row][col] == SEEN) {
        continue;
      }
      Position new_region = std::make_pair(row, col);
      accumulator += get_region_price(garden, new_region, is_part_2);
    }
  }

  return accumulator;
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    std::cerr << "Must provide filepath!" << std::endl;
    return -1;
  }

  const auto garden = get_garden_from_file(argv[1]);

  bool is_part_2 = false;

  Unit accumulator = get_total_price(garden, is_part_2);

  std::cout << "Part 1 price: " << accumulator << std::endl;

  is_part_2 = true;

  accumulator = get_total_price(garden, is_part_2);

  std::cout << "Part 2 price: " << accumulator << std::endl;

  return 0;
}
