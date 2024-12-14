#include <file_io.hpp>

#include <fstream>
#include <iostream>

std::vector<std::string> get_lines_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  std::vector<std::string> output;

  for (std::string line; std::getline(in_stream, line); /*BLANK*/) {
    output.push_back(line);
  }
  return output;
}

void print_lines(const std::vector<std::string> &lines) {

  for (const auto &line : lines) {
    std::cout << line << std::endl;
  }
  std::cout << std::endl;
}
