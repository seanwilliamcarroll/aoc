#include <array>        // for array
#include <core_lib.hpp> // for Coordinate, get_lines_from_file, Tile, Grid
#include <d16.hpp>
#include <deque>     // for deque
#include <limits>    // for numeric_limits
#include <set>       // for set, operator!=, __tree_const_iterator
#include <stdexcept> // for runtime_error
#include <string>    // for basic_string, string, to_string
#include <tuple>     // for get, tuple, make_tuple, operator==
#include <utility>   // for pair, make_pair
#include <vector>    // for vector

namespace d16 {

constexpr Tile START = 'S';
constexpr Tile WALL = '#';
constexpr Tile END = 'E';

using Score = long long;

using Heading = Coordinate;

using HeadingPosition = std::tuple<Heading, Coordinate, Coordinate>;

// constexpr Heading NORTH = 0;
constexpr Heading EAST = 1;
// constexpr Heading SOUTH = 2;
// constexpr Heading WEST = 3;

using Movement = Position;

constexpr Coordinate NUM_DIRECTIONS = 4;

constexpr std::array<Movement, NUM_DIRECTIONS> HEADING_TO_MOVEMENT = {
    {{-1, 0}, {0, 1}, {1, 0}, {0, -1}}};

HeadingPosition find_start_position(const Grid &grid) {
  for (Coordinate row{}; row < grid.size(); ++row) {
    for (Coordinate col{}; col < grid[row].size(); ++col) {
      if (grid[row][col] == START) {
        return std::make_tuple(EAST, row, col);
      }
    }
  }
  throw std::runtime_error("Expected to find start position!");
}

std::pair<Score, Score> find_shortest_path(const Grid &grid) {
  const auto starting_position = find_start_position(grid);

  // At any time, we want to know where we just came from
  using IntermediateResult =
      std::tuple<Score, HeadingPosition, HeadingPosition>;

  std::deque<IntermediateResult> attempts;
  Score lowest_score = std::numeric_limits<Score>::max();
  std::vector<std::vector<std::vector<Score>>> position_to_score(
      NUM_DIRECTIONS,
      std::vector<std::vector<Score>>(
          grid.size(), std::vector<Score>(grid.back().size(), Score())));
  std::vector<std::vector<std::vector<std::set<HeadingPosition>>>>
      position_to_last_position(
          NUM_DIRECTIONS,
          std::vector<std::vector<std::set<HeadingPosition>>>(
              grid.size(),
              std::vector<std::set<HeadingPosition>>(
                  grid.back().size(), std::set<HeadingPosition>{})));
  std::set<HeadingPosition> end_positions;

  attempts.emplace_back(0, starting_position, starting_position);
  while (!attempts.empty()) {
    const auto &attempt = attempts.front();
    const auto [score_so_far, position, last_position] = attempt;
    attempts.pop_front();
    const auto [heading, row, col] = position;
    if (grid[row][col] == END) {
      // Found the end
      if (score_so_far < lowest_score) {
        lowest_score = score_so_far;
        end_positions = std::set<HeadingPosition>{position};
        position_to_last_position[std::get<0>(position)][std::get<1>(position)]
                                 [std::get<2>(position)] =
                                     std::set<HeadingPosition>{last_position};
      } else if (score_so_far == lowest_score) {
        end_positions.insert(position);
        position_to_last_position[std::get<0>(position)][std::get<1>(position)]
                                 [std::get<2>(position)]
                                     .insert(last_position);
      }
      continue;
    }
    const auto last_score =
        position_to_score[std::get<0>(position)][std::get<1>(position)]
                         [std::get<2>(position)];
    if ((last_score != Score() && last_score < score_so_far) ||
        score_so_far >= lowest_score) {
      // visited and we've seen this score or better
      // Or we've already gone past the best so far
      continue;
    }

    const auto [row_incr, col_incr] = HEADING_TO_MOVEMENT[heading];
    const auto new_row = row + row_incr;
    const auto new_col = col + col_incr;

    // Can either take a step forward for cost of 1
    if (grid[new_row][new_col] != WALL) {
      attempts.emplace_back(score_so_far + 1,
                            std::make_tuple(heading, new_row, new_col),
                            position);
    }

    // Or rotate for a cost of 1000
    {
      const auto new_heading = (heading + 1) % NUM_DIRECTIONS;
      attempts.emplace_back(score_so_far + 1000,
                            std::make_tuple(new_heading, row, col), position);
    }
    {
      const auto new_heading = (NUM_DIRECTIONS + heading - 1) % NUM_DIRECTIONS;
      attempts.emplace_back(score_so_far + 1000,
                            std::make_tuple(new_heading, row, col), position);
    }

    if (last_score == Score() || score_so_far < last_score) {
      position_to_score[std::get<0>(position)][std::get<1>(position)]
                       [std::get<2>(position)] = score_so_far;
      position_to_last_position[std::get<0>(position)][std::get<1>(position)]
                               [std::get<2>(position)] =
                                   std::set<HeadingPosition>{last_position};
    } else if (score_so_far == last_score) {
      position_to_last_position[std::get<0>(position)][std::get<1>(position)]
                               [std::get<2>(position)]
                                   .insert(last_position);
    }
  }

  std::set<Position> best_seats;
  std::deque<HeadingPosition> seats;
  seats.insert(seats.end(), end_positions.begin(), end_positions.end());
  while (!seats.empty()) {
    const auto &seat = seats.front();
    best_seats.insert(std::make_pair(std::get<1>(seat), std::get<2>(seat)));
    for (const auto &last_seat : position_to_last_position[std::get<0>(
             seat)][std::get<1>(seat)][std::get<2>(seat)]) {
      if (last_seat != starting_position) {
        seats.push_back(last_seat);
      }
    }
    seats.pop_front();
  }
  best_seats.insert(std::make_pair(std::get<1>(starting_position),
                                   std::get<2>(starting_position)));

  return std::make_pair(lowest_score, best_seats.size());
}

std::string part_1(const std::string &filepath) {

  const auto grid = get_lines_from_file(filepath);

  const auto [part_1, _] = find_shortest_path(grid);

  return std::to_string(part_1);
}

std::string part_2(const std::string &filepath) {
  const auto grid = get_lines_from_file(filepath);

  const auto [_, part_2] = find_shortest_path(grid);

  return std::to_string(part_2);
}

} // namespace d16
