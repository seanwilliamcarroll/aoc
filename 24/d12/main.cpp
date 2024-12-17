#include <algorithm>    // for sort
#include <array>        // for array
#include <core_lib.hpp> // for get_lines_from_file
#include <deque>        // for deque
#include <iostream>     // for basic_ostream, endl, operator<<, cout, cerr
#include <map>          // for map, __map_iterator
#include <set>          // for set, __tree_const_iterator
#include <stddef.h>     // for size_t
#include <string>       // for char_traits, basic_string, string
#include <utility>      // for pair, make_pair
#include <vector>       // for vector

using Garden = Grid;

using Unit = long long;

using Heading = Coordinate;

using HeadingCoordinatePair = std::pair<Heading, Coordinate>;

using HeadingPositionPair = std::pair<Heading, Position>;

constexpr Heading NORTH = 3;
constexpr Heading EAST = 0;
constexpr Heading SOUTH = 1;
constexpr Heading WEST = 2;

constexpr std::array<Position, 4> MOVEMENTS = {
    {{0, 1}, {1, 0}, {0, -1}, {-1, 0}}};

constexpr Tile SEEN = '_';

bool is_same_area(const Garden &garden, const Tile region_type,
                  const Position &position) {
  const auto [row, col] = position;
  return is_in_bounds(garden, position) && region_type == garden[row][col];
}

Unit count_sides(
    const Garden &garden, const Position &region,
    const std::set<HeadingPositionPair> &sides_touching_perimeter) {
  Unit num_sides = 0;

  // Need to keep track of continuous sides

  // Keep track of all tiles on the perimeter

  // merge those that touch into single sides until no more merging?

  // index 0, 2 move columns
  // index 1, 3 move rows
  // Map pair of (movement_index, coordinate not affected by movement (e.g. col
  // if moves columns)) to other coordinate

  // If our heading (movement_index) wants to move columns, then the row is
  // unique per side. The same is true for moving rows and the column being
  // unique. So at least one side per unique heading and coordinate pair plus we
  // need to check if the other coordinates found with this unique heading and
  // coordinate pair are contiguous

  //   3
  // 2-|-0
  //   1

  std::map<HeadingCoordinatePair, std::vector<Coordinate>> side_segments;

  // Collect segments to detect if contiguous
  for (const auto &[movement_index, position] : sides_touching_perimeter) {
    const auto [row, col] = position;
    if (movement_index == EAST || movement_index == WEST) {
      const HeadingCoordinatePair unique_side =
          std::make_pair(movement_index, col);
      side_segments[unique_side].push_back(row);
    } else if (movement_index == NORTH || movement_index == SOUTH) {
      const HeadingCoordinatePair unique_side =
          std::make_pair(movement_index, row);
      side_segments[unique_side].push_back(col);
    }
  }

  // Check if segments are contiguous
  for (auto &[unique_side, segments] : side_segments) {
    std::sort(segments.begin(), segments.end());
    ++num_sides;
    for (size_t index = 1; index < segments.size(); ++index) {
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

  std::set<HeadingPositionPair> sides_touching_perimeter;

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
        sides_touching_perimeter.insert(
            std::make_pair(movement_index, attempt));
        continue;
      }
      if (garden[new_row][new_col] != region_type) {
        sides_touching_perimeter.insert(
            std::make_pair(movement_index, attempt));
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
  } else {
    perimeter = sides_touching_perimeter.size();
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
  greet_day(12);
  if (argc <= 1) {
    std::cerr << "Must provide filepath!" << std::endl;
    return -1;
  }

  const Garden garden = get_lines_from_file(argv[1]);

  bool is_part_2 = false;

  Unit accumulator = get_total_price(garden, is_part_2);

  std::cout << "Part 1: Price: " << accumulator << std::endl;

  is_part_2 = true;

  accumulator = get_total_price(garden, is_part_2);

  std::cout << "Part 2: Price: " << accumulator << std::endl;

  return 0;
}
