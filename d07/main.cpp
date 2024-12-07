#include <deque>    // for deque, __deque_iterator
#include <fstream>  // for basic_ostream, endl, operator<<, basic_istream
#include <iostream> // for cout, cerr
#include <stddef.h> // for size_t
#include <string>   // for char_traits, stoull, string
#include <utility>  // for make_pair, pair
#include <vector>   // for vector

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
  const std::string output_str = std::to_string(left) + std::to_string(right);
  return std::stoull(output_str);
}

bool is_equation_possible(const Equation &equation,
                          const bool is_part_2 = false) {
  const auto &[result, operands] = equation;

  struct IntermediateResult {
    TestValue value_so_far;
    Operands operands_left;
    IntermediateResult(TestValue value_so_far, const Operands &operands)
        : value_so_far(value_so_far)
        , operands_left(operands) {}
  };

  std::deque<IntermediateResult> attempts;
  attempts.emplace_back(TestValue(), operands);
  while (attempts.size() > 0) {
    auto &attempt = attempts.front();
    if (attempt.operands_left.empty()) {
      if (result == attempt.value_so_far) {
        return true;
      }
      // drop attempt
    } else {
      TestValue next_operand = attempt.operands_left.front();
      attempt.operands_left.pop_front();
      TestValue next_value_so_far = attempt.value_so_far + next_operand;
      if (next_value_so_far <= result) {
        IntermediateResult add =
            IntermediateResult(next_value_so_far, attempt.operands_left);
        attempts.push_back(add);
      }
      next_value_so_far = attempt.value_so_far * next_operand;
      if (next_value_so_far <= result) {
        IntermediateResult mul =
            IntermediateResult(next_value_so_far, attempt.operands_left);
        attempts.push_back(mul);
      }

      if (is_part_2) {
        next_value_so_far = do_concat(attempt.value_so_far, next_operand);
        if (next_value_so_far <= result) {
          IntermediateResult concat =
              IntermediateResult(next_value_so_far, attempt.operands_left);
          attempts.push_back(concat);
        }
      }
    }

    attempts.pop_front();
  }

  return false;
}

TestValue sum_valid_results(const Equations &equations,
                            const bool is_part_2 = false) {
  TestValue accumulator{};

  for (const auto &equation : equations) {
    if (is_equation_possible(equation, is_part_2)) {
      const auto &[result, _] = equation;
      accumulator += result;
      std::cout << ".";
    } else {
      std::cout << "X";
    }
    std::cout << std::flush;
  }
  std::cout << std::endl;
  return accumulator;
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    std::cerr << "Must provide filepath!" << std::endl;
    return -1;
  }

  const auto equations = get_equations_from_file(argv[1]);

  bool is_part_2 = false;

  TestValue accumulator = sum_valid_results(equations, is_part_2);

  std::cout << "Part 1 sum: " << accumulator << std::endl;

  is_part_2 = true;
  accumulator = sum_valid_results(equations, is_part_2);

  std::cout << "Part 2 sum: " << accumulator << std::endl;

  return 0;
}
