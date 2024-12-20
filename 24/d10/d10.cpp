#include <d10.hpp>
#include <array>         // for array
#include <core_lib.hpp>  // for Position, Tile, get_lines_from_file, Coordinate
#include <deque>         // for deque
#include <set>           // for set
#include <string>        // for basic_string, string, to_string
#include <utility>       // for pair
#include <vector>        // for vector

namespace d10 {

using ElevationMap = Grid;

using Positions = std::vector<Position>;

constexpr Tile TRAILHEAD = '0';
constexpr Tile TRAILEND = '9';

constexpr std::array<Position, 4> MOVE_DIRECTIONS = {
    {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};

Positions get_trailhead_positions(const ElevationMap &elev_map) {
  Positions trailhead_positions;
  for (Coordinate row{}; row < elev_map.size(); ++row) {
    for (Coordinate col{}; col < elev_map[row].size(); ++col) {
      if (elev_map[row][col] == TRAILHEAD) {
        trailhead_positions.emplace_back(row, col);
      }
    }
  }
  return trailhead_positions;
}

bool is_elevation_gradually_increasing(const Tile start, const Tile end) {
  return (end - start) == 1;
}

Positions get_trailend_positions(const ElevationMap &elev_map,
                                 const Position &trailhead) {

  Positions trailends;

  std::deque<Position> locations_to_check;

  locations_to_check.push_back(trailhead);

  while (!locations_to_check.empty()) {
    const auto [start_row, start_col] = locations_to_check.front();
    locations_to_check.pop_front();

    if (elev_map[start_row][start_col] == TRAILEND) {
      trailends.emplace_back(start_row, start_col);
      continue;
    }
    for (const auto &[move_row, move_col] : MOVE_DIRECTIONS) {
      const auto new_row = start_row + move_row;
      const auto new_col = start_col + move_col;
      if (!is_in_bounds(elev_map, new_row, new_col)) {
        continue;
      }
      if (!is_elevation_gradually_increasing(elev_map[start_row][start_col],
                                             elev_map[new_row][new_col])) {
        continue;
      }
      // In bounds and we are gradually increasing
      locations_to_check.emplace_back(new_row, new_col);
    }
  }

  return trailends;
}

int count_unique_trailheads(const ElevationMap &elev_map) {
  int accumulator = 0;

  const Positions trailhead_positions = get_trailhead_positions(elev_map);

  for (const auto &trailhead_position : trailhead_positions) {
    const auto trailend_positions =
        get_trailend_positions(elev_map, trailhead_position);
    const std::set<Position> unique_positions(trailend_positions.begin(),
                                              trailend_positions.end());
    accumulator += unique_positions.size();
  }

  return accumulator;
}

int count_unique_trails(const ElevationMap &elev_map) {
  int accumulator = 0;

  const Positions trailhead_positions = get_trailhead_positions(elev_map);

  for (const auto &trailhead_position : trailhead_positions) {
    const auto trailend_positions =
        get_trailend_positions(elev_map, trailhead_position);
    accumulator += trailend_positions.size();
  }

  return accumulator;
}

std::string part_1(const std::string &filepath) {

  const ElevationMap elev_map = get_lines_from_file(filepath);

  int accumulator = count_unique_trailheads(elev_map);

  return std::to_string(accumulator);
}

std::string part_2(const std::string &filepath) {

  const ElevationMap elev_map = get_lines_from_file(filepath);


  int accumulator = count_unique_trails(elev_map);
  return std::to_string(accumulator);
}

} // namespace d10
