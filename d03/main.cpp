#include <_ctype.h> // for isdigit
#include <cctype>   // for isdigit
#include <cstddef>  // for size_t
#include <fstream>  // for basic_ostream, endl, operator<<, basic_ifstream
#include <iostream> // for cout, cerr
#include <sstream>  // for basic_stringstream, stringstream
#include <string>   // for char_traits, allocator, string, stoi

std::stringstream get_content_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  std::stringstream output;

  output << in_stream.rdbuf();

  return output;
}

std::size_t get_next_valid_digit(const std::string &input, size_t pos) {

  size_t num_digits{};

  std::string output_str;

  while (num_digits < 3 && std::isdigit(input[pos + num_digits])) {
    output_str.push_back(input[pos + num_digits]);
    ++num_digits;
  }
  return num_digits;
}

int calculate_muls(const std::string &input, bool enable_do = false) {
  size_t pos{};

  int output = 0;

  bool mult_enabled = true;

  while (pos < input.size()) {
    size_t next_mul = input.find("mul(", pos);

    if (enable_do) {
      size_t next_do = input.find("do()", pos);
      if (next_do == std::string::npos && !mult_enabled) {
        return output;
      }
      if (next_do == std::string::npos) {
        next_do = input.size();
      }
      size_t next_dont = input.find("don't()", pos);
      if (next_dont == std::string::npos && mult_enabled) {
        enable_do = false;
      }
      if (next_dont == std::string::npos) {
        next_dont = input.size();
      }

      if (next_mul < next_do && next_mul < next_dont) {
        // Do the parsing
      } else if (next_do < next_mul && next_do < next_dont) {
        // enable
        mult_enabled = true;
        pos = next_do + 4;
        continue;
      } else {
        // disable
        mult_enabled = false;
        pos = next_dont + 6;
        continue;
      }
    }

    if (next_mul == std::string::npos) {
      return output;
    }
    pos = next_mul + 4;

    size_t num_digits = get_next_valid_digit(input, pos);
    if (num_digits == 0) {
      continue;
    }
    int first_term = std::stoi(input.substr(pos, num_digits));
    pos += num_digits;

    if (input[pos] != ',') {
      continue;
    }
    ++pos;

    num_digits = get_next_valid_digit(input, pos);
    if (num_digits == 0) {
      continue;
    }
    int second_term = std::stoi(input.substr(pos, num_digits));
    pos += num_digits;

    if (input[pos] != ')') {
      continue;
    }
    ++pos;

    if (mult_enabled) {
      output += first_term * second_term;
    }
  }
  return output;
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    std::cerr << "Must provide filepath!" << std::endl;
    return -1;
  }

  auto file_stream = get_content_from_file(argv[1]);

  int accumulator = calculate_muls(file_stream.str());

  std::cout << "First part sum: " << accumulator << std::endl;

  accumulator = calculate_muls(file_stream.str(), true);

  std::cout << "First part sum: " << accumulator << std::endl;

  return 0;
}
