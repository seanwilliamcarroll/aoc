#include <_ctype.h>      // for isalnum
#include <cctype>        // for isalnum
#include <fstream>       // for basic_ostream, endl, operator<<, basic_istream
#include <iostream>      // for cout, cerr
#include <set>           // for set
#include <stddef.h>      // for size_t
#include <string>        // for char_traits, basic_string, string
#include <unordered_map> // for __hash_map_iterator, operator==, unordered_map
#include <utility>       // for pair, make_pair
#include <vector>        // for vector

using Coordinate = int;

using Position = std::pair<Coordinate, Coordinate>;

using AntennaMap = std::vector<std::string>;

using Tile = char;

using Antenna = Tile;

using Positions = std::vector<Position>;

using AntennaPositions = std::unordered_map<Antenna, Positions>;

bool is_antenna(const Tile input) { return std::isalnum(input); }

AntennaMap get_antenna_map_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  AntennaMap antenna_map;
  for (std::string line; std::getline(in_stream, line); /*BLANK*/) {
    antenna_map.push_back(line);
  }

  return antenna_map;
}

void print_antenna_map(const AntennaMap &antenna_map) {
  for (const auto &line : antenna_map) {
    std::cout << line << std::endl;
  }
  std::cout << std::endl;
}

AntennaPositions get_antenna_positions(const AntennaMap &antenna_map) {
  AntennaPositions antenna_positions;
  for (Coordinate row_index = 0; row_index < antenna_map.size(); ++row_index) {
    for (Coordinate col_index = 0; col_index < antenna_map[row_index].size();
         ++col_index) {
      const Tile current_tile = antenna_map[row_index][col_index];
      if (!is_antenna(current_tile)) {
        continue;
      }
      const Antenna current_antenna = current_tile;
      auto map_iter = antenna_positions.find(current_antenna);
      if (map_iter == antenna_positions.end()) {
        antenna_positions[current_antenna] = Positions{{row_index, col_index}};
      } else {
        map_iter->second.emplace_back(row_index, col_index);
      }
    }
  }

  return antenna_positions;
}

Coordinate get_antinode_diff(const Coordinate base, const Coordinate second) {
  return (-(second - base));
}

Position get_antinode_position_diff(const Position &base,
                                    const Position &second) {
  const auto [base_row, base_col] = base;
  const auto [second_row, second_col] = second;
  Coordinate antinode_row = get_antinode_diff(base_row, second_row);
  Coordinate antinode_col = get_antinode_diff(base_col, second_col);
  return std::make_pair(antinode_row, antinode_col);
}

Position get_antinode_position(const Position &base, const Position &second) {
  const auto [base_row, base_col] = base;
  const auto [antinode_diff_row, antinode_diff_col] =
      get_antinode_position_diff(base, second);
  return std::make_pair(base_row + antinode_diff_row,
                        base_col + antinode_diff_col);
}

bool is_in_bounds(const AntennaMap &antenna_map, const Position &position) {
  const auto [position_row, position_col] = position;
  return (position_row >= 0 && position_row < antenna_map.size() &&
          position_col >= 0 && position_col < antenna_map.back().size());
}

int count_unique_antinode_positions(const AntennaMap &antenna_map,
                                    const bool is_part_2 = false) {
  std::set<Position> unique_positions;

  const AntennaPositions antenna_positions = get_antenna_positions(antenna_map);

  for (const auto &[antenna, positions] : antenna_positions) {
    for (size_t base_index = 0; base_index < positions.size(); ++base_index) {
      for (size_t second_index = 0; second_index < positions.size();
           ++second_index) {
        if (base_index == second_index) {
          continue;
        }
        if (!is_part_2) {
          // Part 1
          const auto antinode_position = get_antinode_position(
              positions[base_index], positions[second_index]);
          if (is_in_bounds(antenna_map, antinode_position)) {
            unique_positions.insert(antinode_position);
          }
        } else {
          // Part 2
          // All pairs of antennae will have antinodes on the antennae positions
          // themselves
          unique_positions.insert(positions[base_index]);
          unique_positions.insert(positions[second_index]);
          const auto antinode_position_diff = get_antinode_position_diff(
              positions[base_index], positions[second_index]);
          auto [antinode_row_diff, antinode_col_diff] = antinode_position_diff;
          auto [base_row, base_col] = positions[base_index];
          base_row += antinode_row_diff;
          base_col += antinode_col_diff;
          auto antinode_position = std::make_pair(base_row, base_col);
          while (is_in_bounds(antenna_map, antinode_position)) {
            unique_positions.insert(antinode_position);
            base_row += antinode_row_diff;
            base_col += antinode_col_diff;
            antinode_position = std::make_pair(base_row, base_col);
          }
        }
      }
    }
  }

  return unique_positions.size();
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    std::cerr << "Must provide filepath!" << std::endl;
    return -1;
  }

  const auto antenna_map = get_antenna_map_from_file(argv[1]);

  bool is_part_2 = false;

  int accumulator = count_unique_antinode_positions(antenna_map, is_part_2);

  std::cout << "Part 1 total antinodes: " << accumulator << std::endl;

  is_part_2 = true;

  accumulator = count_unique_antinode_positions(antenna_map, is_part_2);

  std::cout << "Part 2 total antinodes: " << accumulator << std::endl;

  return 0;
}
