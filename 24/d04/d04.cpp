#include <algorithm>    // for reverse
#include <core_lib.hpp> // for Grid, get_lines_from_file
#include <d04.hpp>
#include <initializer_list> // for initializer_list
#include <stddef.h>         // for size_t
#include <string>           // for basic_string, string, to_string, swap
#include <utility>          // for pair
#include <vector>           // for vector

namespace d04 {

int count_xmas_samx(const Grid &lines) {
  int accumulator = 0;
  for (const auto &line : lines) {
    for (const auto &search_word : {"XMAS", "SAMX"}) {
      size_t pos{};
      while (pos + 4 <= line.size()) {
        size_t search_pos = line.find(search_word, pos);
        if (search_pos == std::string::npos) {
          search_pos = line.size();
        }
        if (search_pos < line.size()) {
          ++accumulator;
          pos = search_pos + 4;
        } else {
          pos = line.size();
        }
      }
    }
  }
  return accumulator;
}

Grid transpose(const Grid &lines) {
  size_t longest_line{};
  for (const auto &line : lines) {
    if (longest_line < line.size()) {
      longest_line = line.size();
    }
  }

  Grid output(longest_line);
  for (const auto &line : lines) {
    for (size_t character_index = 0; character_index < line.size();
         ++character_index) {
      output[character_index].push_back(line[character_index]);
    }
  }
  return output;
}

Grid lower_left_to_upper_right_diagonal(const Grid &lines) {
  Grid output;

  for (size_t row_index = 0; row_index < lines.size(); ++row_index) {
    output.emplace_back();
    for (size_t col_index = 0; col_index <= row_index; ++col_index) {
      output.back() += lines[row_index - col_index][col_index];
    }
  }

  for (size_t row_index_plus_one = lines.size(); row_index_plus_one > 0;
       --row_index_plus_one) {
    size_t row_index = row_index_plus_one - 1;
    size_t row_inverse = lines.size() - 1 - row_index;
    size_t col_count{};
    output.emplace_back();
    for (size_t col_index = 1 + row_inverse;
         col_index < lines[row_index].size(); ++col_index) {
      output.back() += lines[lines.size() - col_count - 1][col_index];
      ++col_count;
    }
  }

  return output;
}

Grid flip_up_and_down(const Grid &lines) {
  Grid output(lines);
  std::reverse(output.begin(), output.end());
  return output;
}

Grid upper_left_to_lower_right_diagonal(const Grid &lines) {
  return lower_left_to_upper_right_diagonal(flip_up_and_down(lines));
}

bool in_bounds(int index, int max_size) {
  return index >= 0 && index < max_size;
}

int count_x_shaped_mas(const Grid &lines) {

  const std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>
      x_mas_directions = {{{-1, -1}, {1, 1}}, {{1, -1}, {-1, 1}}};

  int count = 0;

  for (int row_index = 0; row_index < lines.size(); ++row_index) {
    for (int col_index = 0; col_index < lines[row_index].size(); ++col_index) {
      const char character = lines[row_index][col_index];

      if (character != 'A') {
        continue;
      }

      // Found an A
      // TODO: Probably a more elegant solution....
      int direction_count = 0;
      for (const auto &direction_set : x_mas_directions) {
        const auto [direction_a, direction_b] = direction_set;

        const auto [row_incr_a, col_incr_a] = direction_a;
        const auto [row_incr_b, col_incr_b] = direction_b;

        const auto new_row_index_a = row_index + row_incr_a;
        const auto new_col_index_a = col_index + col_incr_a;
        const auto new_row_index_b = row_index + row_incr_b;
        const auto new_col_index_b = col_index + col_incr_b;

        if (!in_bounds(new_row_index_a, lines.size()) ||
            !in_bounds(new_col_index_a, lines[new_row_index_a].size()) ||
            !in_bounds(new_row_index_b, lines.size()) ||
            !in_bounds(new_col_index_b, lines[new_row_index_b].size())) {
          continue;
        }
        // Can check
        if ((lines[new_row_index_a][new_col_index_a] == 'M' &&
             lines[new_row_index_b][new_col_index_b] == 'S') ||
            (lines[new_row_index_a][new_col_index_a] == 'S' &&
             lines[new_row_index_b][new_col_index_b] == 'M')) {
          ++direction_count;
        }
      }
      if (direction_count == 2) {
        ++count;
      }
    }
  }

  return count;
}

int count_all_xmas_part_1(const Grid &lines) {
  int accumulator = 0;

  int horizontal = count_xmas_samx(lines);

  accumulator += horizontal;

  Grid transposed_lines = transpose(lines);

  int vertical = count_xmas_samx(transposed_lines);

  accumulator += vertical;

  Grid lower_left_to_upper_right_diagonal_lines =
      lower_left_to_upper_right_diagonal(lines);

  int lower_left_to_upper_right =
      count_xmas_samx(lower_left_to_upper_right_diagonal_lines);

  accumulator += lower_left_to_upper_right;

  Grid upper_left_to_lower_right_diagonal_lines =
      upper_left_to_lower_right_diagonal(lines);

  int upper_left_to_lower_right =
      count_xmas_samx(upper_left_to_lower_right_diagonal_lines);

  accumulator += upper_left_to_lower_right;

  return accumulator;
}

std::string part_1(const std::string &filepath) {
  const auto lines = get_lines_from_file(filepath);

  int accumulator = count_all_xmas_part_1(lines);

  return std::to_string(accumulator);
}

std::string part_2(const std::string &filepath) {
  const auto lines = get_lines_from_file(filepath);

  int accumulator = count_x_shaped_mas(lines);

  return std::to_string(accumulator);
}

} // namespace d04
