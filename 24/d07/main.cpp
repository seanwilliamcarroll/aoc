#include <cmath>        // for floor, log10
#include <core_lib.hpp> // for greet_day
#include <deque>        // for deque, __deque_iterator
#include <fstream>      // for basic_ostream, operator<<, endl, basic_istream
#include <iostream>     // for cout, cerr
#include <stddef.h>     // for size_t
#include <string>       // for char_traits, stoull, string
#include <utility>      // for make_pair, pair
#include <vector>       // for vector

using TestValue = unsigned long long;

using Operands = std::deque<TestValue>;

using Equation = std::pair<TestValue, Operands>;

using Equations = std::vector<Equation>;

Equations get_equations_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  Equations equations;
  for (std::string line; std::getline(in_stream, line); /*BLANK*/) {
    TestValue result{};
    Operands operands{};
    size_t index{};
    std::string result_str;
    while (line[index] != ':') {
      result_str.push_back(line[index]);
      ++index;
    }
    result = std::stoull(result_str);
    // index points to colon
    ++index;
    // index points to space
    ++index;
    // index points to first character of operands

    while (index < line.size()) {
      std::string operand_str;
      while (index < line.size() && line[index] != ' ') {
        operand_str.push_back(line[index]);
        ++index;
      }
      operands.push_back(std::stoull(operand_str));
      // index points to space or end of string
      ++index;
    }
    equations.push_back(std::make_pair(result, operands));
  }

  return equations;
}

void print_equation(const Equation &equation) {
  const auto &[result, operands] = equation;
  std::cout << result << " = ";
  for (const auto operand : operands) {
    std::cout << operand << " ";
  }
  std::cout << std::endl;
}

void print_equations(const Equations &equations) {
  for (const auto &equation : equations) {
    print_equation(equation);
  }
  std::cout << std::endl;
}

TestValue do_concat(const TestValue left, const TestValue right) {
  TestValue num_digits = TestValue(std::floor(std::log10(right))) + 1;
  return (std::pow(10ULL, num_digits) * left) + right;
}

bool is_equation_possible(const Equation &equation,
                          const bool is_part_2 = false) {
  const auto &[result, operands] = equation;

  using IntermediateResult = std::pair<TestValue, size_t>;

  std::deque<IntermediateResult> attempts;
  attempts.emplace_back(TestValue(), 0);
  while (attempts.size() > 0) {
    const auto &attempt = attempts.front();
    const auto [value_so_far, operand_index] = attempt;
    attempts.pop_front();
    if (operand_index == operands.size()) {
      if (result == value_so_far) {
        return true;
      }
      // drop attempt
      continue;
    }
    const TestValue next_operand = operands[operand_index];
    TestValue next_value_so_far = value_so_far + next_operand;
    if (next_value_so_far <= result) {
      attempts.emplace_back(next_value_so_far, operand_index + 1);
    }
    next_value_so_far = value_so_far * next_operand;
    if (next_value_so_far <= result) {
      attempts.emplace_back(next_value_so_far, operand_index + 1);
    }

    if (!is_part_2) {
      continue;
    }
    next_value_so_far = do_concat(value_so_far, next_operand);
    if (next_value_so_far <= result) {
      attempts.emplace_back(next_value_so_far, operand_index + 1);
    }
  }

  return false;
}

TestValue sum_valid_results(const Equations &equations, const bool is_part_2,
                            const bool do_display = true) {
  TestValue accumulator{};

  for (const auto &equation : equations) {
    if (is_equation_possible(equation, is_part_2)) {
      const auto &[result, _] = equation;
      accumulator += result;
      if (do_display) {
        std::cout << ".";
      }
    } else {
      if (do_display) {
        std::cout << "X";
      }
    }
    if (do_display) {
      std::cout << std::flush;
    }
  }
  if (do_display) {
    std::cout << std::endl;
  }
  return accumulator;
}

int main(int argc, char *argv[]) {
  greet_day(7);
  if (argc <= 1) {
    std::cerr << "Must provide filepath!" << std::endl;
    return -1;
  }

  const auto equations = get_equations_from_file(argv[1]);

  bool is_part_2 = false;

  TestValue accumulator = sum_valid_results(equations, is_part_2);

  std::cout << "Part 1: Sum: " << accumulator << std::endl;

  is_part_2 = true;
  accumulator = sum_valid_results(equations, is_part_2);

  std::cout << "Part 2: Sum: " << accumulator << std::endl;

  return 0;
}
