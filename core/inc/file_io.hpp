#pragma once

#include <string>
#include <vector>

std::vector<std::string> get_lines_from_file(const std::string &filepath);

void print_lines(const std::vector<std::string> &);
