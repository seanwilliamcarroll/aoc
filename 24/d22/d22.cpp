#include <d22.hpp>
#include <fstream>  // for basic_istream, basic_ifstream, ifstream
#include <map>      // for map
#include <set>      // for set, __tree_const_iterator
#include <stddef.h> // for size_t
#include <tuple>    // for make_tuple, tuple
#include <utility>  // for make_pair, pair
#include <vector>   // for vector

// clang-format off
namespace d22 {
// clang-format on

using Value = long long;

using Values = std::vector<Value>;

Values get_values_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  Values values;
  Value value;
  while (in_stream >> value) {
    values.push_back(value);
  }
  return values;
}

Value mix(const Value left, const Value right) { return left ^ right; }

Value prune(const Value value) { return value % (16777216ULL); }

Value mult_mix_prune(const Value value, const Value mult) {
  Value output = value;
  Value to_mix;
  to_mix = output * mult;
  output = mix(output, to_mix);
  output = prune(output);
  return output;
}

Value div_mix_prune(const Value value, const Value div) {
  Value output = value;
  Value to_mix;
  to_mix = output / div;
  output = mix(output, to_mix);
  output = prune(output);
  return output;
}

Value generate_next_secret_number(const Value value) {
  Value output = value;
  output = mult_mix_prune(output, 64ULL);
  output = div_mix_prune(output, 32ULL);
  output = mult_mix_prune(output, 2048ULL);
  return output;
}

Value calc_nth_secret_number(const Value value, const size_t n) {
  Value output = value;
  for (size_t index{}; index < n; ++index) {
    output = generate_next_secret_number(output);
  }
  return output;
}

Value get_total_sum_of_2000th_secret_number(const Values &values) {
  Value output{};
  for (const auto value : values) {
    output += calc_nth_secret_number(value, 2000);
  }
  return output;
}

std::pair<Values, Values>
calc_2000_secret_numbers_and_change_sequence(const Value value) {
  Values prices;
  prices.reserve(2000);
  Values changes;
  changes.reserve(2000);
  Value last_value = value;
  Value last_price = value % 10;
  for (size_t index{}; index < 2000; ++index) {
    Value next_value = generate_next_secret_number(last_value);
    Value next_price = next_value % 10;
    prices.push_back(next_price);
    changes.push_back(next_price - last_price);
    last_value = next_value;
    last_price = next_price;
  }
  return std::make_pair(prices, changes);
}

Value get_most_bananas(const Values values) {
  using PriceChangeSequence = std::tuple<Value, Value, Value, Value>;
  std::set<PriceChangeSequence> all_sequences;
  std::map<PriceChangeSequence, Value> first_values;
  std::set<PriceChangeSequence> seen;
  for (const auto value : values) {
    const auto [prices, changes] =
        calc_2000_secret_numbers_and_change_sequence(value);
    seen.clear();
    for (size_t index = 3; index < changes.size(); ++index) {
      PriceChangeSequence sequence =
          std::make_tuple(changes[index - 3], changes[index - 2],
                          changes[index - 1], changes[index]);
      if (seen.count(sequence) > 0) {
        continue;
      }
      seen.insert(sequence);
      all_sequences.insert(sequence);
      first_values[sequence] += prices[index];
    }
  }

  Value most_bananas{};
  for (const auto &sequence : all_sequences) {
    Value banana_count = first_values[sequence];
    if (banana_count > most_bananas) {
      most_bananas = banana_count;
    }
  }
  return most_bananas;
}

std::string part_1(const std::string &filepath) {
  const auto values = get_values_from_file(filepath);

  return std::to_string(get_total_sum_of_2000th_secret_number(values));
}

std::string part_2(const std::string &filepath) {
  const auto values = get_values_from_file(filepath);

  return std::to_string(get_most_bananas(values));
}

// clang-format off
} // namespace d22
// clang-format on
