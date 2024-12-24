#include <core_lib.hpp> // for do_assert
#include <d24.hpp>
#include <deque>            // for deque
#include <fstream>          // for basic_istream, basic_ifstream, getline
#include <initializer_list> // for initializer_list
#include <map>              // for __map_iterator, operator==, map, operator!=
#include <memory>           // for unique_ptr
#include <stddef.h>         // for size_t
#include <stdexcept>        // for runtime_error
#include <string>           // for allocator, char_traits, string, basic_st...
#include <vector>           // for vector

// clang-format off
namespace d24 {
// clang-format on

using FinalDecimal = unsigned long long;

using Value = bool;

using Name = std::string;

enum Operation { AND, OR, XOR };

struct Node {
  Name m_name;

  Node(const Name &name)
      : m_name(name) {}

  virtual ~Node() = default;

  virtual Value get_value() const = 0;
};

using Nodes = std::map<Name, std::unique_ptr<Node>>;

struct ValueNode : public Node {
  Value m_value;

  ValueNode(const Name &name, const Value value)
      : Node(name)
      , m_value(value) {}

  Value get_value() const override {
    return m_value;
  }
};

struct IntermediateNode : public Node {
  Operation m_operation;
  Node *m_input_a;
  Node *m_input_b;
  bool m_is_solved;
  Value m_final_value;

  IntermediateNode(const Name &name, const Operation operation, Node *input_a,
                   Node *input_b)
      : Node(name)
      , m_operation(operation)
      , m_input_a(input_a)
      , m_input_b(input_b)
      , m_is_solved(false)
      , m_final_value{} {}

  Value get_value() const override {
    do_assert(m_is_solved, "Expected this node to have already been solved!");
    return m_final_value;
  }
};

void parse_value_node(Nodes &nodes, const std::string &input) {
  size_t colon_pos = input.find(":");
  do_assert(colon_pos != std::string::npos,
            "Expected to find colon character on this line!");
  Name name = input.substr(0, colon_pos);
  size_t value_pos = colon_pos + 2;
  Value value = (input[value_pos] == '1');

  const auto &iterator = nodes.find(name);
  do_assert(iterator == nodes.end(),
            std::string("Already found node with name: ") + name);

  nodes.emplace(name, new ValueNode(name, value));
}

bool parse_intermediate_node(Nodes &nodes, const std::string &input) {
  size_t pos{};
  pos = input.find(" ", pos);
  do_assert(pos != std::string::npos,
            "Expected to find space character on this line!");
  Name input_a = input.substr(0, pos);
  ++pos;

  Operation operation;
  if (input[pos] == 'A') {
    operation = Operation::AND;
    pos += 3 + 1;
  } else if (input[pos] == 'O') {
    operation = Operation::OR;
    pos += 2 + 1;
  } else if (input[pos] == 'X') {
    operation = Operation::XOR;
    pos += 3 + 1;
  } else {
    throw std::runtime_error("Unknown operation!");
  }
  size_t arrow_pos = input.find("->", pos);
  do_assert(arrow_pos != std::string::npos,
            "Expected to find \"->\" on this line!");
  --arrow_pos;
  Name input_b = input.substr(pos, arrow_pos - pos);

  pos = arrow_pos + 1 + 2 + 1;
  Name output = input.substr(pos, input.size() - pos);

  std::vector<Node *> input_ptrs;
  for (const auto &name : {input_a, input_b}) {
    auto iterator = nodes.find(name);
    if (iterator == nodes.end()) {
      return false;
    } else {
      input_ptrs.push_back(iterator->second.get());
    }
  }

  auto iterator = nodes.find(output);
  if (iterator != nodes.end()) {
  }
  nodes.emplace(output, new IntermediateNode(output, operation, input_ptrs[0],
                                             input_ptrs[1]));
  return true;
}

Nodes get_nodes_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  Nodes nodes;

  bool consumed_all_values = false;
  std::deque<std::string> lines_to_parse;
  for (std::string line; std::getline(in_stream, line); /*BLANK*/) {
    if (line.size() == 0) {
      consumed_all_values = true;
      continue;
    }
    if (!consumed_all_values) {
      parse_value_node(nodes, line);
    } else {
      lines_to_parse.push_back(line);
    }
  }

  while (!lines_to_parse.empty()) {
    const auto &line = lines_to_parse.front();
    if (!parse_intermediate_node(nodes, line)) {
      lines_to_parse.push_back(line);
    }
    lines_to_parse.pop_front();
  }

  return nodes;
}

Value solve_node(IntermediateNode *);

Value get_value_from_node(Node *node) {
  ValueNode *value_node = dynamic_cast<ValueNode *>(node);
  if (value_node != nullptr) {
    return value_node->m_value;
  }
  IntermediateNode *int_node = dynamic_cast<IntermediateNode *>(node);
  do_assert(int_node != nullptr, "Unable to correctly cast!");
  Value value = solve_node(int_node);
  int_node->m_is_solved = true;
  return value;
}

Value solve_node(IntermediateNode *node) {

  Value value_a = get_value_from_node(node->m_input_a);

  Value value_b = get_value_from_node(node->m_input_b);

  Value output{};
  switch (node->m_operation) {
  case Operation::AND: {
    output = value_a & value_b;
    break;
  }
  case Operation::OR: {
    output = value_a | value_b;
    break;
  }
  case Operation::XOR: {
    output = value_a ^ value_b;
    break;
  }
  default: {
    throw std::runtime_error("Unexpected operation!");
  }
  }
  return output;
}

void solve_for_values(Nodes &nodes) {
  for (auto &[name, node] : nodes) {
    IntermediateNode *int_node = dynamic_cast<IntermediateNode *>(node.get());
    if (int_node == nullptr) {
      continue;
    }
    if (int_node->m_is_solved) {
      continue;
    }
    int_node->m_final_value = solve_node(int_node);
    int_node->m_is_solved = true;
  }
}

FinalDecimal get_final_z_decimal_value(Nodes &nodes) {
  FinalDecimal output{};
  for (auto &[name, node] : nodes) {
    if (name.find("z") != 0) {
      continue;
    }
    if (!((name.size() == 3 && name[1] >= '0' && name[1] <= '9' &&
           name[2] >= 0 && name[2] <= '9'))) {
      continue;
    }
    FinalDecimal index = (10 * (name[1] - '0')) + (name[2] - '0');
    output |= FinalDecimal(node->get_value()) << index;
  }

  return output;
}

std::string part_1(const std::string &filepath) {
  auto nodes = get_nodes_from_file(filepath);

  solve_for_values(nodes);

  const auto output = get_final_z_decimal_value(nodes);

  return std::to_string(output);
}

std::string part_2(const std::string &filepath) {
  return std::string();
}

// clang-format off
} // namespace d24
// clang-format on
