#include <d22.hpp>
#include <fstream>  // for basic_istream, basic_ifstream, ifstream
#include <stddef.h> // for size_t
#include <vector>   // for vector

// clang-format off
namespace d22 {
// clang-format on

using Value = unsigned long long;

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

std::string part_1(const std::string &filepath) {
  const auto values = get_values_from_file(filepath);

  return std::to_string(get_total_sum_of_2000th_secret_number(values));
}

std::string part_2(const std::string &filepath) { return std::string(); }

// clang-format off
} // namespace d22
// clang-format on
