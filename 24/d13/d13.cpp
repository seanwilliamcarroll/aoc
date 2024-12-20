#include <d13.hpp>
#include <algorithm> // for min_element
#include <cmath>     // for llround
#include <core_lib.hpp>
#include <fstream>   // for basic_ostream, operator<<, endl, basic_istream
#include <iostream>  // for cout, cerr
#include <stddef.h>  // for size_t
#include <stdexcept> // for runtime_error
#include <string>    // for char_traits, stoll, string
#include <utility>   // for make_pair, pair
#include <vector>    // for vector

namespace d13 {


using Increment = Position;

using Tokens = long long;

struct Equation {
  Increment m_ButtonA;
  Increment m_ButtonB;
  Position m_Prize;
};

constexpr Tokens BUTTON_A_COST = 3;
constexpr Tokens BUTTON_B_COST = 1;

using Equations = std::vector<Equation>;

Increment parse_button(const std::string &line) {
  size_t pos = line.find_first_of("X+");
  if (pos == std::string::npos) {
    throw std::runtime_error("Unexpected end of line!");
  }
  // Skip over "X+"
  pos += 2;
  size_t comma = line.find_first_of(",", pos);
  if (comma == std::string::npos) {
    throw std::runtime_error("Unexpected end of line!");
  }
  Coordinate x_increment = std::stoll(line.substr(pos, comma - pos));
  // length of ", Y+", laziness on my part
  pos = comma + 4;
  Coordinate y_increment = std::stoll(line.substr(pos, line.size() - pos));
  return std::make_pair(x_increment, y_increment);
}

Position parse_prize(const std::string &line) {
  size_t pos = line.find_first_of("X=");
  if (pos == std::string::npos) {
    throw std::runtime_error("Unexpected end of line!");
  }
  // Skip over "X="
  pos += 2;
  size_t comma = line.find_first_of(",", pos);
  if (comma == std::string::npos) {
    throw std::runtime_error("Unexpected end of line!");
  }
  Coordinate x_coordinate = std::stoll(line.substr(pos, comma - pos));
  // length of ", Y=", laziness on my part
  pos = comma + 4;
  Coordinate y_coordinate = std::stoll(line.substr(pos, line.size() - pos));
  return std::make_pair(x_coordinate, y_coordinate);
}

Equations get_equations_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  Equations equations;
  for (std::string line; std::getline(in_stream, line); /*BLANK*/) {
    if (line.size() == 0) {
      continue;
    }
    Equation equation;
    equation.m_ButtonA = parse_button(line);
    std::getline(in_stream, line);
    equation.m_ButtonB = parse_button(line);
    std::getline(in_stream, line);
    equation.m_Prize = parse_prize(line);
    equations.push_back(equation);
  }

  return equations;
}

void print_equation(const Equation &equation) {
  std::cout << "Button A: (" << equation.m_ButtonA.first << ", "
            << equation.m_ButtonA.second << ") ";
  std::cout << "Button B: (" << equation.m_ButtonB.first << ", "
            << equation.m_ButtonB.second << ") ";
  std::cout << "Prize: (" << equation.m_Prize.first << ", "
            << equation.m_Prize.second << ") ";
  std::cout << std::endl;
}

void print_equations(const Equations &equations) {
  for (const auto &equation : equations) {
    print_equation(equation);
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

Tokens find_intersection_cost(const Equation &equation) {
  // Want to return the cost at the point of intersection, if there is a point
  // of intersection

  const auto [a_x, a_y] = equation.m_ButtonA;
  const auto [b_x, b_y] = equation.m_ButtonB;
  const auto [p_x, p_y] = equation.m_Prize;

  if (a_x == 0 || a_y == 0 || b_x == 0 || b_y == 0) {
    return 0;
  }

  const double a_x_double = equation.m_ButtonA.first;
  const double b_x_double = equation.m_ButtonB.first;
  const double p_x_double = equation.m_Prize.first;

  const double a_y_double = equation.m_ButtonA.second;
  const double b_y_double = equation.m_ButtonB.second;
  const double p_y_double = equation.m_Prize.second;

  // Want to find spot that
  // a_x * num_a_presses + b_x * num_b_presses == p_x
  // a_y * num_a_presses + b_y * num_b_presses == p_y
  // intersect

  auto num_a_presses_double =
      (a_y_double / b_y_double) - (a_x_double / b_x_double);
  if (num_a_presses_double == 0) {
    return 0;
  }
  num_a_presses_double =
      ((p_y_double / b_y_double) - (p_x_double / b_x_double)) /
      (num_a_presses_double);

  auto num_b_presses_double = (p_x_double / b_x_double) -
                              (a_x_double / b_x_double) * num_a_presses_double;

  // Convert back to integer and confirm the numbers are positive and yield the
  // correct prize position

  const Coordinate num_a_presses = std::llround(num_a_presses_double);
  const Coordinate num_b_presses = std::llround(num_b_presses_double);

  if ((num_a_presses > 0) && (num_b_presses > 0) &&
      (a_x * num_a_presses + b_x * num_b_presses == p_x) &&
      (a_y * num_a_presses + b_y * num_b_presses == p_y)) {
    return num_a_presses * BUTTON_A_COST + num_b_presses * BUTTON_B_COST;
  }
  return 0;
}

template <typename IntegralType>
bool is_evenly_divisible(const IntegralType numerator,
                         const IntegralType denominator) {
  return (denominator != 0 && numerator % denominator == 0);
}

Tokens get_minimized_cost(const Equation &equation) {

  const auto [a_x, a_y] = equation.m_ButtonA;
  const auto [b_x, b_y] = equation.m_ButtonB;
  const auto [p_x, p_y] = equation.m_Prize;

  std::vector<Tokens> total_costs;

  // Find intersections

  // Set A presses to 0, find intersection
  if (is_evenly_divisible(b_x, p_x) && is_evenly_divisible(b_y, p_y) &&
      (p_x / b_x) == (p_y / b_y)) {
    total_costs.push_back(BUTTON_B_COST * (p_y / b_y));
  }

  // Set B presses to 0, find intersection
  if (is_evenly_divisible(a_x, p_x) && is_evenly_divisible(a_y, p_y) &&
      (p_x / a_x) == (p_y / a_y)) {
    total_costs.push_back(BUTTON_A_COST * (p_y / a_y));
  }

  // Find intersection of A and B presses
  total_costs.push_back(find_intersection_cost(equation));

  if (total_costs.empty()) {
    return 0;
  } else if (total_costs.size() == 1) {
    return total_costs.front();
  }

  // Take the minimum
  return *std::min_element(total_costs.begin(), total_costs.end());
}

Tokens get_total_minimized_cost(const Equations &equations) {
  Tokens cost = 0;
  for (const auto &equation : equations) {
    cost += get_minimized_cost(equation);
  }
  return cost;
}

Equations convert_equations_part_2(const Equations &equations) {
  Equations new_equations(equations);
  for (auto &new_equation : new_equations) {
    new_equation.m_Prize.first += 10000000000000;
    new_equation.m_Prize.second += 10000000000000;
  }
  return new_equations;
}

  
std::string part_1(const std::string &filepath) {
  const auto equations = get_equations_from_file(filepath);

  Tokens accumulator = get_total_minimized_cost(equations);

  return std::to_string(accumulator);
}

std::string part_2(const std::string &filepath) {

  const auto equations = get_equations_from_file(filepath);

  const auto new_equations = convert_equations_part_2(equations);

  Tokens accumulator = get_total_minimized_cost(new_equations);

  return std::to_string(accumulator);
}

} // namespace d13
