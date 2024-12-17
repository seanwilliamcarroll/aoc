#include <algorithm>    // for min_element
#include <array>        // for array
#include <core_lib.hpp> // for do_assert, greet_day
#include <deque>        // for deque
#include <fstream>      // for basic_ostream, operator<<, endl, basic_istream
#include <iostream>     // for cout, cerr
#include <sstream>      // for basic_stringstream, stringstream
#include <stddef.h>     // for size_t
#include <stdexcept>    // for runtime_error
#include <string>       // for char_traits, string, allocator, operator+
#include <vector>       // for vector

using Value = long long;

using Register = Value;

using Instruction = Value;

using Instructions = std::vector<Instruction>;

using Values = std::vector<Value>;

constexpr Instruction ADV = 0;
constexpr Instruction BXL = 1;
constexpr Instruction BST = 2;
constexpr Instruction JNZ = 3;
constexpr Instruction BXC = 4;
constexpr Instruction OUT = 5;
constexpr Instruction BDV = 6;
constexpr Instruction CDV = 7;

constexpr size_t NUM_INSTRUCTIONS = 8;

constexpr std::array<const char *, NUM_INSTRUCTIONS> INSTRUCTION_NAMES = {
    "ADV", "BXL", "BST", "JNZ", "BXC", "OUT", "BDV", "CDV"};

struct c_ProgramState {
  Register m_A;
  Register m_B;
  Register m_C;

  size_t m_ProgramCounter;

  Instructions m_Instructions;

  Values m_Output;

  Value combo_operand(const Value operand) const;

  size_t simulate_instruction(const Instruction instruction,
                              const Value operand);

  void simulate();
};

void print_instructions(const Instructions &instructions) {
  for (size_t index{}; index < instructions.size() - 1; index += 2) {
    std::cout << INSTRUCTION_NAMES[instructions[index]] << " "
              << instructions[index + 1] << std::endl;
  }
}

template <typename SequenceType>
std::string sequence_to_string(const SequenceType &input) {
  std::stringstream output;
  bool first = true;
  for (const auto element : input) {
    if (!first) {
      output << ",";
    }
    output << Value(element);
    first = false;
  }
  return output.str();
}

Value c_ProgramState::combo_operand(const Value operand) const {
  if (operand <= 3) {
    return operand;
  }
  if (operand == 4) {
    return m_A;
  }
  if (operand == 5) {
    return m_B;
  }
  if (operand == 6) {
    return m_C;
  }
  throw std::runtime_error(std::string("Invalid combo operand: ") +
                           std::to_string(operand));
}

size_t c_ProgramState::simulate_instruction(const Instruction instruction,
                                          const Value operand) {
  switch (instruction) {
  case ADV: {
    m_A = m_A / (1LL << combo_operand(operand));
    break;
  };
  case BXL: {
    m_B = m_B ^ operand;
    break;
  };
  case BST: {
    m_B = combo_operand(operand) % 8;
    break;
  };
  case JNZ: {
    if (m_A != 0) {
      m_ProgramCounter = operand;
      return 0;
    }
    break;
  };
  case BXC: {
    m_B = m_B ^ m_C;
    break;
  };
  case OUT: {
    m_Output.push_back(combo_operand(operand) % 8);
    break;
  };
  case BDV: {
    m_B = m_A / (1LL << combo_operand(operand));
    break;
  };
  case CDV: {
    m_C = m_A / (1LL << combo_operand(operand));
    break;
  };
  default: {
    throw std::runtime_error(std::string("Unexpected instruction: ") +
                             std::to_string(int(instruction)));
    break;
  };
  }

  return 2;
}

void c_ProgramState::simulate() {
  while (m_ProgramCounter < m_Instructions.size() - 1) {
    Instruction current_instruction = m_Instructions[m_ProgramCounter];
    Value operand = m_Instructions[m_ProgramCounter + 1];
    size_t next_offset = simulate_instruction(current_instruction, operand);
    m_ProgramCounter += next_offset;
  }
}

template <typename SequenceType>
void print_sequence(const SequenceType &input, const std::string &identifier) {
  std::cout << identifier << "[" << input.size()
            << "]: " << sequence_to_string(input) << std::endl;
}

void print_program_state(const c_ProgramState &program_state) {
  std::cout << "ProgramCounter: " << program_state.m_ProgramCounter << std::endl
            << std::endl;

  std::cout << "Register A: " << program_state.m_A << std::endl;
  std::cout << "Register B: " << program_state.m_B << std::endl;
  std::cout << "Register C: " << program_state.m_C << std::endl;

  std::cout << std::endl;

  print_sequence(program_state.m_Instructions, "Instructions");
  print_sequence(program_state.m_Output, "Output");
  std::cout << std::endl;
}

Register parse_register(const std::string &line) {
  size_t pos = line.find(":");
  // skip over colon and space
  pos += 2;

  return std::stoll(line.substr(pos, line.size() - pos));
}

Instructions parse_instructions(const std::string &line) {
  size_t pos = line.find(":");
  // skip over colon and space
  pos += 2;

  Instructions instructions;
  while (pos < line.size()) {
    size_t comma = line.find(",", pos);
    if (comma == std::string::npos) {
      comma = line.size();
    }
    Instruction instruction = std::stoll(line.substr(pos, comma - pos));
    instructions.push_back(instruction);
    pos = comma + 1;
  }
  return instructions;
}

c_ProgramState get_program_state_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  c_ProgramState program_state;

  std::string line;

  if (!std::getline(in_stream, line)) {
    throw std::runtime_error("Unexpected EOF!");
  }
  program_state.m_A = parse_register(line);

  if (!std::getline(in_stream, line)) {
    throw std::runtime_error("Unexpected EOF!");
  }
  program_state.m_B = parse_register(line);

  if (!std::getline(in_stream, line)) {
    throw std::runtime_error("Unexpected EOF!");
  }
  program_state.m_C = parse_register(line);

  // get blank line
  if (!std::getline(in_stream, line)) {
    throw std::runtime_error("Unexpected EOF!");
  } else if (!line.empty()) {
    throw std::runtime_error("Expected blank line!!");
  }

  // get instructions
  if (!std::getline(in_stream, line)) {
    throw std::runtime_error("Unexpected EOF!");
  }
  program_state.m_Instructions = parse_instructions(line);

  return program_state;
}

c_ProgramState simulate_program(const c_ProgramState &input_program_state) {
  c_ProgramState program_state(input_program_state);

  program_state.simulate();

  return program_state;
}

void add_new_values_to_try(const Value new_value,
                           std::deque<Value> &next_to_try) {
  for (Value next_value = new_value; next_value < new_value + 8; ++next_value) {
    next_to_try.push_front(next_value);
  }
}

int compare_from_back(const Values &shorter, const Values &longer) {
  // Expect size of shorter to be less than or equal to size of longer
  // Returns number of matches from back to front of shorter compared to back to
  // front of longer, returning when a mismatch is found
  int num_match = 0;
  for (int index = shorter.size() - 1; index >= 0; --index) {
    const int offset = longer.size() - shorter.size();
    if (shorter[index] == longer[index + offset]) {
      ++num_match;
    } else {
      break;
    }
  }
  return num_match;
}

Value find_lowest_value_for_quine(const c_ProgramState &input_program_state) {
  // Need to do some form of search....

  // Do DFS and look for all possible values
  // Since we are mod 8 on every output, try 8 values at a time, looking for
  // best match When we match end of sequence, multiple this value by 8 and add
  // to front of list to try

  Values output;

  std::deque<Value> next_to_try;
  add_new_values_to_try(0, next_to_try);
  while (!next_to_try.empty()) {
    // Get next value to try
    Value A_reg = next_to_try.front();
    next_to_try.pop_front();

    // Try it
    c_ProgramState program_state(input_program_state);
    program_state.m_A = A_reg;
    program_state.simulate();

    // Evaluate it
    const int num_matches = compare_from_back(
        program_state.m_Output, input_program_state.m_Instructions);
    if (num_matches == input_program_state.m_Instructions.size()) {
      output.push_back(A_reg);
    } else if (num_matches == program_state.m_Output.size()) {
      add_new_values_to_try(A_reg * 8, next_to_try);
    }
  }

  do_assert(!output.empty(), "Expected to find a solution!!");

  return *std::min_element(output.begin(), output.end());
}

int main(int argc, char *argv[]) {
  greet_day(17);
  if (argc <= 1) {
    std::cerr << "Must provide filepath!" << std::endl;
    return -1;
  }

  const auto program_state = get_program_state_from_file(argv[1]);

  const auto output_program_state = simulate_program(program_state);

  std::cout << "Part 1: " << sequence_to_string(output_program_state.m_Output)
            << std::endl;

  Value accumulator = find_lowest_value_for_quine(program_state);

  std::cout << "Part 2: " << accumulator << std::endl;

  return 0;
}
