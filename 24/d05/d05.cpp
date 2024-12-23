#include <__hash_table> // for __hash_const_iterator
#include <_ctype.h>     // for isdigit
#include <cctype>       // for isdigit
#include <d05.hpp>
#include <fstream>       // for basic_istream, basic_ifstream, getline, bas...
#include <iostream>      // for cout
#include <stddef.h>      // for size_t
#include <stdexcept>     // for runtime_error
#include <string>        // for char_traits, string, stoi, to_string
#include <unordered_map> // for operator==, __hash_map_const_iterator, __ha...
#include <unordered_set> // for unordered_set
#include <utility>       // for make_pair, pair
#include <vector>        // for vector

namespace d05 {

// A RuleSet consists of all page numbers that may not come before the page in
// question
using RuleSet = std::unordered_set<int>;
// AllRules maps page numbers to their RuleSets
using AllRules = std::unordered_map<int, RuleSet>;

using PageOrder = std::vector<int>;
using PageOrders = std::vector<PageOrder>;

std::pair<int, int> parse_rule(const std::string &line) {

  std::string first_str;
  size_t pos{};
  while (std::isdigit(line[pos])) {
    first_str += line[pos];
    ++pos;
  }

  if (line[pos] != '|') {
    throw std::runtime_error("Unexpected!");
  }
  ++pos;

  std::string second_str;
  while (pos < line.size()) {
    second_str += line[pos];
    ++pos;
  }

  return std::make_pair(std::stoi(first_str), std::stoi(second_str));
}

PageOrder parse_page_order(const std::string &line) {
  PageOrder output;

  size_t pos{};
  while (pos < line.size()) {

    std::string num_str;
    while (std::isdigit(line[pos])) {
      num_str += line[pos];
      ++pos;
    }

    output.push_back(std::stoi(num_str));

    if (pos == line.size()) {
      break;
    }

    if (line[pos] != ',') {
      throw std::runtime_error("Unexpected!");
    }
    ++pos;
  }

  return output;
}

std::pair<AllRules, PageOrders>
get_rules_and_pages_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  AllRules all_rules;
  // Collect RuleSet first
  for (std::string line; std::getline(in_stream, line); /*BLANK*/) {
    if (line.size() == 0) {
      break;
    }
    const auto [first, second] = parse_rule(line);

    auto rule_set_iter = all_rules.find(first);
    if (rule_set_iter == all_rules.end()) {
      all_rules.emplace(first, RuleSet{second});
    } else {
      rule_set_iter->second.insert(second);
    }
  }

  PageOrders page_orders;
  // Collect PageOrders
  for (std::string line; std::getline(in_stream, line); /*BLANK*/) {
    page_orders.push_back(parse_page_order(line));
  }

  return std::make_pair(all_rules, page_orders);
}

void print_page_order(const PageOrder &page_order) {
  for (const auto page_number : page_order) {
    std::cout << page_number << " ";
  }
  std::cout << std::endl;
}

std::pair<PageOrders, PageOrders>
find_valid_and_invalid_page_orders(const PageOrders &page_orders,
                                   const AllRules &all_rules) {
  PageOrders valid_page_orders;
  PageOrders invalid_page_orders;

  for (const auto &page_order : page_orders) {
    RuleSet seen_so_far;
    bool is_valid = true;
    for (const auto page_number : page_order) {
      // Get all pages that cannot come before this page, see if we've seen them
      // yet
      auto rule_iter = all_rules.find(page_number);
      if (rule_iter == all_rules.end()) {
        // No rule, keep going
        seen_so_far.insert(page_number);
        continue;
      }
      const auto rule_set = rule_iter->second;
      for (const auto previous_page : seen_so_far) {
        if (rule_set.count(previous_page) > 0) {
          is_valid = false;
          break;
        }
      }
      if (!is_valid) {
        break;
      }

      // Insert our page
      seen_so_far.insert(page_number);
    }

    // Sum middle element if valid
    if (is_valid) {
      valid_page_orders.push_back(page_order);
    } else {
      invalid_page_orders.push_back(page_order);
    }
  }

  return std::make_pair(valid_page_orders, invalid_page_orders);
}

PageOrders fix_invalid_page_orders(const PageOrders &page_orders,
                                   const AllRules &all_rules) {
  PageOrders valid_page_orders;

  for (auto page_order : page_orders) {
    PageOrder fixed_page_order;
    for (const auto page_number : page_order) {
      auto rule_iter = all_rules.find(page_number);
      if (rule_iter == all_rules.end()) {
        // No rule, keep going
        fixed_page_order.push_back(page_number);
        continue;
      }

      // can we insert the current page into the fixed order at this point?
      const auto rule_set = rule_iter->second;
      bool did_insert = false;
      for (size_t fixed_page_order_index = 0;
           fixed_page_order_index < fixed_page_order.size();
           ++fixed_page_order_index) {
        const auto previous_page = fixed_page_order[fixed_page_order_index];
        if (rule_set.count(previous_page) > 0) {
          fixed_page_order.insert(
              fixed_page_order.begin() + fixed_page_order_index, page_number);
          did_insert = true;
          break;
        }
      }
      if (!did_insert) {
        fixed_page_order.push_back(page_number);
      }
    }
    valid_page_orders.push_back(fixed_page_order);
  }

  return valid_page_orders;
}

int sum_middle_pages(const PageOrders &page_orders) {
  int accumulator = 0;

  for (const auto &page_order : page_orders) {
    accumulator += page_order[page_order.size() / 2];
  }

  return accumulator;
}

std::string part_1(const std::string &filepath) {

  const auto [all_rules, page_orders] = get_rules_and_pages_from_file(filepath);

  const auto [valid_page_orders, invalid_page_orders] =
      find_valid_and_invalid_page_orders(page_orders, all_rules);

  int accumulator = sum_middle_pages(valid_page_orders);

  return std::to_string(accumulator);
}

std::string part_2(const std::string &filepath) {

  const auto [all_rules, page_orders] = get_rules_and_pages_from_file(filepath);

  const auto [valid_page_orders, invalid_page_orders] =
      find_valid_and_invalid_page_orders(page_orders, all_rules);

  PageOrders fixed_page_orders =
      fix_invalid_page_orders(invalid_page_orders, all_rules);

  int accumulator = sum_middle_pages(fixed_page_orders);

  return std::to_string(accumulator);
}

} // namespace d05
