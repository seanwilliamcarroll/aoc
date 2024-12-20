#include <d09.hpp>
#include <deque>    // for deque, __deque_iterator
#include <fstream>  // for basic_istream, basic_ostream, endl, operator<<
#include <iostream> // for cout
#include <stddef.h> // for size_t
#include <string>   // for char_traits, string, to_string
#include <utility>  // for pair, make_pair, swap
#include <vector>   // for vector

namespace d09 {

using NumBlocks = unsigned long long;

using FileId = long long;

using DefragMap = std::vector<NumBlocks>;

using DiskLayout = std::vector<FileId>;

using Index = long long;
using Length = long long;

using SpanPosition = std::pair<Index, Length>;

using SpanPositions = std::deque<SpanPosition>;

DefragMap get_defrag_map_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  std::string defrag_map_str;
  for (std::string line; std::getline(in_stream, line); /*BLANK*/) {
    defrag_map_str += line;
  }

  DefragMap defrag_map;
  for (const auto character : defrag_map_str) {
    defrag_map.push_back(character - '0');
  }

  return defrag_map;
}

void print_defrag_map(const DefragMap &defrag_map) {
  for (const auto element : defrag_map) {
    std::cout << char(element + '0');
  }
  std::cout << std::endl;
}

std::pair<NumBlocks, NumBlocks>
get_num_file_blocks_num_free_blocks(const DefragMap &defrag_map) {
  NumBlocks file_blocks{};
  NumBlocks free_blocks{};

  bool is_file = true;
  for (const auto num_blocks : defrag_map) {
    if (is_file) {
      file_blocks += num_blocks;
    } else {
      free_blocks += num_blocks;
    }
    is_file = !is_file;
  }

  return std::make_pair(file_blocks, free_blocks);
}

DiskLayout get_layout(const DefragMap &defrag_map) {
  const auto [num_file_blocks, num_free_blocks] =
      get_num_file_blocks_num_free_blocks(defrag_map);
  DiskLayout layout(num_file_blocks + num_free_blocks, (-1));

  bool is_file = true;
  size_t index{};
  FileId file_id = 0;
  for (const auto num_blocks : defrag_map) {
    size_t begin_index = index;
    while (index < begin_index + num_blocks) {
      if (is_file) {
        layout[index] = file_id;
      }
      ++index;
    }
    if (is_file) {
      ++file_id;
    }
    is_file = !is_file;
  }

  return layout;
}

NumBlocks calc_checksum(const DiskLayout &layout) {
  NumBlocks checksum = 0;
  for (size_t index{}; index < layout.size(); ++index) {
    const auto block_id = layout[index];
    if (block_id >= 0) {
      checksum += (block_id * index);
    }
  }
  return checksum;
}

void print_layout(const DiskLayout &layout) {

  for (const auto block_id : layout) {
    if (block_id >= 0) {
      std::cout << block_id % 10;
    } else {
      std::cout << ".";
    }
  }
  std::cout << std::endl;
}

NumBlocks get_part_1_checksum(const DefragMap &defrag_map) {
  DiskLayout layout = get_layout(defrag_map);

  size_t begin_index{};
  size_t end_index = layout.size() - 1;
  while (begin_index < end_index) {
    if (layout[begin_index] < 0 && layout[end_index] >= 0) {
      std::swap(layout[begin_index], layout[end_index]);
      ++begin_index;
      --end_index;
    } else if (layout[begin_index] < 0) {
      --end_index;
    } else {
      ++begin_index;
    }
  }

  NumBlocks checksum = calc_checksum(layout);

  return checksum;
}

std::pair<SpanPositions, SpanPositions>
get_file_and_free_positions(const DefragMap &defrag_map) {
  SpanPositions file_span_positions;
  SpanPositions free_span_positions;
  Index current_position{};
  for (size_t index = 0; index < defrag_map.size(); ++index) {
    if (index % 2 == 0) {
      file_span_positions.emplace_back(current_position, defrag_map[index]);
    } else {
      free_span_positions.emplace_back(current_position, defrag_map[index]);
    }
    current_position += defrag_map[index];
  }
  return std::make_pair(file_span_positions, free_span_positions);
}

bool try_merge(SpanPositions &free_span_positions, Index free_index) {
  auto &[free_start_index, free_length] = free_span_positions[free_index];
  if (free_index == 0) {
    return false;
  }
  auto &[last_free_start_index, last_free_length] =
      free_span_positions[free_index - 1];
  if (last_free_start_index + last_free_length == free_start_index) {
    last_free_length += free_length;
    free_span_positions.erase(free_span_positions.begin() + free_index);
    return true;
  }
  return false;
}

bool try_remove_zero(SpanPositions &free_span_positions, Index free_index) {
  auto [_, free_length] = free_span_positions[free_index];
  if (free_length == 0) {
    free_span_positions.erase(free_span_positions.begin() + free_index);
    return true;
  }
  return false;
}

void do_swap(DiskLayout &layout, Index file_start_index, Length file_length,
             Index free_start_index) {
  for (Index index = 0; index < file_length; ++index) {
    std::swap(layout[file_start_index + index],
              layout[free_start_index + index]);
  }
}

NumBlocks get_part_2_checksum(const DefragMap &defrag_map) {
  DiskLayout layout = get_layout(defrag_map);

  auto [file_span_positions, free_span_positions] =
      get_file_and_free_positions(defrag_map);

  for (Index file_id = file_span_positions.size() - 1; file_id >= 0;
       --file_id) {
    const auto [file_start_index, file_length] = file_span_positions[file_id];
    Index free_index = 0;
    while (free_index < free_span_positions.size()) {
      if (try_remove_zero(free_span_positions, free_index)) {
        // stay on this index
        continue;
      }
      if (try_merge(free_span_positions, free_index)) {
        // Need to go back one for the merge
        free_index -= 1;
        continue;
      }
      auto &[free_start_index, free_length] = free_span_positions[free_index];
      if (free_start_index > file_start_index) {
        // we want to move things forward only
        break;
      }
      if (file_length > free_length) {
        // not big enough
        ++free_index;
        continue;
      }
      // found a space we can swap with
      do_swap(layout, file_start_index, file_length, free_start_index);
      free_start_index = free_start_index + file_length;
      free_length = free_length - file_length;
      break;
    }
  }

  NumBlocks checksum = calc_checksum(layout);

  return checksum;
}

std::string part_1(const std::string &filepath) {
  const auto defrag_map = get_defrag_map_from_file(filepath);

  NumBlocks accumulator = get_part_1_checksum(defrag_map);

  return std::to_string(accumulator);
}

std::string part_2(const std::string &filepath) {
  const auto defrag_map = get_defrag_map_from_file(filepath);

  NumBlocks accumulator = get_part_2_checksum(defrag_map);

  return std::to_string(accumulator);
}

} // namespace d09
