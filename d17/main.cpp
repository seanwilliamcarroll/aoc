#include <core_lib.hpp> // for greet_day
#include <fstream>      // for basic_ostream, operator<<, endl, basic_istream
#include <iostream>     // for cout, cerr
#include <stddef.h>     // for size_t
#include <stdexcept>    // for runtime_error
#include <stdint.h>     // for uint8_t
#include <string>       // for char_traits, string, operator+, to_string, stoi
#include <vector>       // for vector

using Value = long long;

using Register = Value;

using Instruction = uint8_t;

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

struct ProgramState {
  Register m_A;
  Register m_B;
  Register m_C;

  size_t m_ProgramCounter;

  Instructions m_Instructions;

  Values m_Output;

  Value combo_operand(const Value operand) {
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

  size_t simulate_instruction(const Instruction instruction,
                              const Value operand) {
    switch (instruction) {
    case ADV: {
      // std::cout << "ADV: " << operand << std::endl;
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

  void simulate() {
    // std::cout << "Next PC: " << m_ProgramCounter << std::endl;
    while (m_ProgramCounter < m_Instructions.size() - 1) {
      Instruction current_instruction = m_Instructions[m_ProgramCounter];
      Value operand = m_Instructions[m_ProgramCounter + 1];
      size_t next_offset = simulate_instruction(current_instruction, operand);
      m_ProgramCounter += next_offset;
      // std::cout << "Next PC: " << m_ProgramCounter << std::endl;
    }
  }
};

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
    Instruction instruction = std::stoi(line.substr(pos, comma - pos));
    instructions.push_back(instruction);
    pos = comma + 1;
  }
  return instructions;
}

ProgramState get_program_state_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  ProgramState program_state;

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

template <typename SequenceType>
void print_sequence(const SequenceType& input, const std::string& identifier) {
  bool first = true;
  std::cout << identifier << "[" << input.size() << "]: ";
  for (const auto element : input) {
    if (!first) {
      std::cout << ",";
    }
    std::cout << Value(element);
    first = false;
  }

  std::cout << std::endl;

}

void print_program_state(const ProgramState &program_state) {
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



ProgramState simulate_program(const ProgramState &input_program_state) {
  ProgramState program_state(input_program_state);

  program_state.simulate();

  return program_state;
}

int main(int argc, char *argv[]) {
  greet_day(17);
  if (argc <= 1) {
    std::cerr << "Must provide filepath!" << std::endl;
    return -1;
  }

  const auto program_state = get_program_state_from_file(argv[1]);

  print_program_state(program_state);

  const auto output_program_state = simulate_program(program_state);

  print_program_state(output_program_state);

  std::cout << "Part 1: ";
  print_sequence(output_program_state.m_Output, "Output");


  return 0;
}
