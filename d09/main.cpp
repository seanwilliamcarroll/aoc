#include <fstream>  // for basic_ostream, endl, operator<<, basic_istream
#include <iostream> // for cout, cerr
#include <stddef.h> // for size_t
#include <string>   // for char_traits, string
#include <utility>  // for swap, make_pair, pair
#include <vector>   // for vector

using NumBlocks = unsigned long long;

using FileId = long long;

using DefragMap = std::vector<NumBlocks>;

using DiskLayout = std::vector<FileId>;

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

  // print_layout(layout);

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

  // print_layout(layout);

  NumBlocks checksum = calc_checksum(layout);

  return checksum;
}

NumBlocks get_part_2_checksum(const DefragMap &defrag_map) {
  DiskLayout layout = get_layout(defrag_map);

  // print_layout(layout);

  FileId end_index = layout.size() - 1;
  FileId current_id;
  bool found_ids = false;
  while (end_index > 0) {
    // Find next file
    if (layout[end_index] < 0) {
      --end_index;
      continue;
    }
    if (!found_ids) {
      current_id = layout[end_index];
      found_ids = true;
    } else {
      if (layout[end_index] != current_id - 1) {
        --end_index;
        continue;
      }
      current_id = layout[end_index];
    }
    // std::cout << "Current file: " << current_id << std::endl;
    // Found file
    FileId start_block_index(end_index);
    while (layout[end_index] == layout[start_block_index] &&
           start_block_index >= 0) {
      --start_block_index;
    }
    ++start_block_index;
    FileId length = end_index - start_block_index + 1;
    // std::cout << "Id: " << layout[start_block_index] << " ["
    //           << start_block_index << ", " << end_index
    //           << "] Length: " << length << std::endl;

    // Find large enough free space
    FileId start_free_index = 0;
    bool did_swap = false;
    while (!did_swap && start_free_index < start_block_index) {
      if (layout[start_free_index] >= 0) {
        ++start_free_index;
        continue;
      }
      // Found free space, get length
      FileId end_free_index(start_free_index);
      while (layout[end_free_index] < 0) {
        ++end_free_index;
      }
      --end_free_index;

      if ((end_free_index - start_free_index + 1) >= length) {
        for (FileId index = 0; index < length; ++index) {
          // if (layout[start_free_index + index] >= 0) {
          //   std::cout << "ERROR" << std::endl;
          // }
          // if (layout[start_block_index + index] < 0) {
          //   std::cout << "ERROR" << std::endl;
          // }
          std::swap(layout[start_free_index + index],
                    layout[start_block_index + index]);
        }
        did_swap = true;
        // std::cout << "SWAPPED: " << current_id << " from: ["
        //           << start_block_index << ", " << end_index << "] to ["
        //           << start_free_index << ", " << end_free_index << "]"
        //           << std::endl;
        break;
      } else {
        // Continue
        start_free_index = end_free_index + 1;
      }
    }
    // if (did_swap) {
    //   // std::cout << "SWAPPED: " << current_id << std::endl;
    // } else {
    //   std::cout << "NO SWAP: " << current_id << " length: " << length
    //             << std::endl;
    // }
    end_index = start_block_index - 1;
  }

  // print_layout(layout);

  NumBlocks checksum = calc_checksum(layout);

  return checksum;
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    std::cerr << "Must provide filepath!" << std::endl;
    return -1;
  }

  const auto defrag_map = get_defrag_map_from_file(argv[1]);

  // print_defrag_map(defrag_map);

  NumBlocks accumulator = get_part_1_checksum(defrag_map);

  std::cout << "Part 1 checksum: " << accumulator << std::endl;

  accumulator = get_part_2_checksum(defrag_map);

  std::cout << "Part 2 checksum: " << accumulator << std::endl;

  return 0;
}
