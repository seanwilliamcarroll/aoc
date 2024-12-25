#include <algorithm>    // for set_intersection, sort
#include <core_lib.hpp> // for do_assert
#include <d24.hpp>
#include <deque>            // for deque
#include <fstream>          // for basic_ostream, operator<<, basic_istream
#include <initializer_list> // for initializer_list
#include <iostream>         // for cout
#include <iterator>         // for back_insert_iterator, back_inserter, next
#include <map>              // for __map_iterator, operator==, __map_const_...
#include <memory>           // for unique_ptr
#include <numeric>          // for accumulate
#include <set>              // for set, __tree_const_iterator, operator!=
#include <stddef.h>         // for size_t
#include <stdexcept>        // for runtime_error
#include <string>           // for char_traits, string, allocator, basic_st...
#include <utility>          // for make_pair, swap, pair
#include <vector>           // for vector

// clang-format off
namespace d24 {
// clang-format on

using FinalDecimal = unsigned long long;

using Value = bool;

using Name = std::string;

enum Operation { AND, OR, XOR };

std::string operation_to_string(const Operation operation) {
  switch (operation) {
  case Operation::AND:
    return "AND";
  case Operation::OR:
    return "OR";
  case Operation::XOR:
    return "XOR";
  default:
    throw std::runtime_error("Unknown operation!");
  }
}

struct IntermediateNode;

struct Node {
  Name m_name;
  std::set<IntermediateNode *> m_output_nodes;

  Node(const Name &name)
      : m_name(name) {}

  virtual ~Node() = default;

  virtual Value get_value() const = 0;

  virtual void print() const = 0;
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

  void print() const override {
    std::cout << m_name << " = " << m_value << std::endl;
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

  void print() const override {
    std::cout << m_input_a->m_name << " " << operation_to_string(m_operation)
              << " " << m_input_b->m_name << " -> " << m_name << std::endl;
    m_input_a->print();
    m_input_b->print();
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

  nodes.emplace(output, new IntermediateNode(output, operation, input_ptrs[0],
                                             input_ptrs[1]));
  input_ptrs[0]->m_output_nodes.insert(
      dynamic_cast<IntermediateNode *>(nodes.at(output).get()));
  input_ptrs[1]->m_output_nodes.insert(
      dynamic_cast<IntermediateNode *>(nodes.at(output).get()));
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

std::vector<Node *> find_ultimate_source_nodes(IntermediateNode *int_node) {
  Node *input_a = int_node->m_input_a;
  Node *input_b = int_node->m_input_b;
  std::vector<Node *> output;
  for (auto &input_node : {input_a, input_b}) {
    if (input_node->m_name[0] == 'x' || input_node->m_name[0] == 'y') {
      output.push_back(input_node);
    } else {
      IntermediateNode *input_int_node =
          dynamic_cast<IntermediateNode *>(input_node);
      do_assert(input_int_node != nullptr,
                "Expected to find intermediate node!");
      auto sources = find_ultimate_source_nodes(input_int_node);
      output.insert(output.end(), sources.begin(), sources.end());
    }
  }
  return output;
}

std::vector<Node *> find_ultimate_output_nodes(IntermediateNode *int_node) {
  std::vector<Node *> output;
  for (auto &output_node : int_node->m_output_nodes) {
    if (output_node->m_name[0] == 'z') {
      output.push_back(output_node);
    } else {
      IntermediateNode *output_int_node =
          dynamic_cast<IntermediateNode *>(output_node);
      do_assert(output_int_node != nullptr,
                "Expected to find intermediate node!");
      auto outputs = find_ultimate_output_nodes(output_int_node);
      output.insert(output.end(), outputs.begin(), outputs.end());
    }
  }
  return output;
}

std::vector<Node *> find_intermediate_source_nodes(IntermediateNode *int_node) {
  Node *input_a = int_node->m_input_a;
  Node *input_b = int_node->m_input_b;
  std::vector<Node *> output;
  for (auto &input_node : {input_a, input_b}) {
    if (input_node->m_name[0] != 'x' && input_node->m_name[0] != 'y') {
      output.push_back(input_node);
      IntermediateNode *input_int_node =
          dynamic_cast<IntermediateNode *>(input_node);
      do_assert(input_int_node != nullptr,
                "Expected to find intermediate node!");
      auto sources = find_intermediate_source_nodes(input_int_node);
      output.insert(output.end(), sources.begin(), sources.end());
    }
  }
  return output;
}

void print_source_nodes(const Nodes &nodes) {
  for (const auto &[name, node] : nodes) {
    if (name[0] != 'z') {
      continue;
    }
    IntermediateNode *int_node = dynamic_cast<IntermediateNode *>(node.get());
    do_assert(int_node != nullptr, "Expected to find intermediate node!");
    auto sources = find_ultimate_source_nodes(int_node);
    std::sort(sources.begin(), sources.end());
    std::cout << int_node->m_name << " : ";
    for (const auto &source_node : sources) {
      std::cout << source_node->m_name << " ";
    }
    std::cout << std::endl;
  }
}

std::string to_padded_string(const int value) {
  if (value < 10) {
    return std::string("0") + std::to_string(value);
  }
  return std::to_string(value);
}

std::pair<IntermediateNode *, IntermediateNode *>
get_xor_and_and_output(Node *left, Node *right) {
  auto &left_outputs = left->m_output_nodes;
  auto &right_outputs = right->m_output_nodes;

  std::vector<Node *> common_outputs;
  std::set_intersection(left_outputs.begin(), left_outputs.end(),
                        right_outputs.begin(), right_outputs.end(),
                        std::back_inserter(common_outputs));

  do_assert(common_outputs.size() == 2, "Expected two common outputs!");
  Node *output_xor = common_outputs.front();
  IntermediateNode *output_xor_int =
      dynamic_cast<IntermediateNode *>(output_xor);
  do_assert(output_xor_int != nullptr,
            "Expected this to be an intermediate node!");
  Node *output_and = common_outputs.back();
  IntermediateNode *output_and_int =
      dynamic_cast<IntermediateNode *>(output_and);
  do_assert(output_and_int != nullptr,
            "Expected this to be an intermediate node!");
  if (output_xor_int->m_operation == Operation::AND &&
      output_and_int->m_operation == Operation::XOR) {
    std::swap(output_xor_int, output_and_int);
  } else if (output_xor_int->m_operation == Operation::XOR &&
             output_and_int->m_operation == Operation::AND) {
    // nothing
  } else {
    throw std::runtime_error("Unexpected operations");
  }
  return std::make_pair(output_xor_int, output_and_int);
}

IntermediateNode *get_single_common_output(Node *left, Node *right) {
  auto &left_outputs = left->m_output_nodes;
  auto &right_outputs = right->m_output_nodes;

  std::vector<Node *> common_outputs;
  std::set_intersection(left_outputs.begin(), left_outputs.end(),
                        right_outputs.begin(), right_outputs.end(),
                        std::back_inserter(common_outputs));

  if (common_outputs.size() != 1) {
    return nullptr;
  }
  Node *output = common_outputs.front();
  IntermediateNode *output_int = dynamic_cast<IntermediateNode *>(output);
  do_assert(output_int != nullptr, "Expected this to be an intermediate node!");
  return output_int;
}

std::set<Name> find_mismatched_nodes(const Nodes &nodes) {
  // Not sure if this is complete per se, but it solves the problem at least

  std::set<Name> mismatched;
  std::set<IntermediateNode *> ands;
  std::set<IntermediateNode *> ors;
  std::set<IntermediateNode *> xors;

  FinalDecimal max_z_value = 0;

  for (const auto &[name, node] : nodes) {
    IntermediateNode *int_node = dynamic_cast<IntermediateNode *>(node.get());
    if (int_node == nullptr) {
      continue;
    }
    if (int_node->m_operation == Operation::AND) {
      ands.insert(int_node);
    }
    if (int_node->m_operation == Operation::OR) {
      ors.insert(int_node);
    }
    if (int_node->m_operation == Operation::XOR) {
      xors.insert(int_node);
    }
    if (name[0] == 'z') {
      FinalDecimal z_value = (10 * (name[1] - '0')) + (name[2] - '0');
      if (z_value > max_z_value) {
        max_z_value = z_value;
      }
    }
  }

  for (int index = 0; index <= max_z_value; ++index) {
    // z outputs can only come from XORs (or an OR for the final bit)
    Node *z_bit_node =
        nodes.at(std::string("z") + to_padded_string(index)).get();
    IntermediateNode *z_bit = dynamic_cast<IntermediateNode *>(z_bit_node);
    do_assert(z_bit != nullptr, "Expected valid pointer!");
    if ((index < max_z_value && z_bit->m_operation != Operation::XOR) ||
        (index == max_z_value && z_bit->m_operation != Operation::OR)) {
      mismatched.insert(z_bit->m_name);
    }
  }

  // Go through all XORs, they must have an X or Y as source or Z as output
  for (const IntermediateNode *xor_node : xors) {
    const Name &output_name = xor_node->m_name;
    const Name &input_a_name = xor_node->m_input_a->m_name;
    const Name &input_b_name = xor_node->m_input_b->m_name;
    if (!(output_name[0] == 'z' ||
          (input_a_name[0] == 'x' || input_b_name[0] == 'x') ||
          (input_a_name[0] == 'y' || input_b_name[0] == 'y'))) {
      mismatched.insert(output_name);
    }
    for (const IntermediateNode *output_node : xor_node->m_output_nodes) {
      if (output_node->m_operation == Operation::OR) {
        mismatched.insert(output_name);
      }
    }
  }

  // ANDs can only output to ORs
  for (const IntermediateNode *and_node : ands) {
    const Name &output_name = and_node->m_name;
    const Name &input_a_name = and_node->m_input_a->m_name;
    const Name &input_b_name = and_node->m_input_b->m_name;
    if (input_a_name == "x00" || input_b_name == "x00") {
      continue;
    }
    for (const IntermediateNode *output_node : and_node->m_output_nodes) {
      if (output_node->m_operation != Operation::OR) {
        mismatched.insert(output_name);
      }
    }
  }

  // ORs can only output to XORs or ANDs
  for (const IntermediateNode *or_node : ors) {
    const Name &output_name = or_node->m_name;
    for (const IntermediateNode *output_node : or_node->m_output_nodes) {
      if (output_node->m_operation == Operation::OR) {
        mismatched.insert(output_name);
      }
    }
  }

  return mismatched;
}

std::string part_1(const std::string &filepath) {
  auto nodes = get_nodes_from_file(filepath);

  solve_for_values(nodes);

  const auto output = get_final_z_decimal_value(nodes);

  return std::to_string(output);
}

std::string part_2(const std::string &filepath) {
  auto nodes = get_nodes_from_file(filepath);

  auto mismatched_set = find_mismatched_nodes(nodes);
  std::vector<Name> mismatched(mismatched_set.begin(), mismatched_set.end());

  std::sort(mismatched.begin(), mismatched.end());
  const std::string mismatched_names = std::accumulate(
      std::next(mismatched.begin()), mismatched.end(), mismatched.front(),
      [](const std::string &left, const std::string &right) {
        return left + std::string(",") + right;
      });

  return mismatched_names;
}

// clang-format off
} // namespace d24
// clang-format on
