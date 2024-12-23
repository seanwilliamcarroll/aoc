#include <algorithm> // for sort
#include <d23.hpp>
#include <fstream>  // for basic_istream, basic_ifstream, getline, ifstream
#include <map>      // for map
#include <set>      // for set, __tree_const_iterator, operator!=
#include <stddef.h> // for size_t
#include <string>   // for char_traits, string, basic_string, to_string
#include <tuple>    // for make_tuple, tuple
#include <utility>  // for pair
#include <vector>   // for vector

// clang-format off
namespace d23 {
// clang-format on

using Computer = std::string;
using Computers = std::vector<Computer>;
using ConnectedGroup = std::set<Computer>;
using Connection = std::pair<Computer, Computer>;
using Connections = std::vector<Connection>;

using AdjacencyList = std::map<Computer, ConnectedGroup>;

using ConnectedGroups = std::set<ConnectedGroup>;

using ThreeSet = std::tuple<Computer, Computer, Computer>;

using ThreeSets = std::set<ThreeSet>;

struct Network {
  Computers m_computers;
  AdjacencyList m_edges;
};

Connections get_connections_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  Connections connections;

  for (std::string line; std::getline(in_stream, line); /*BLANK*/) {
    size_t hyphen_pos = line.find("-");
    Computer computer_a = line.substr(0, hyphen_pos);
    Computer computer_b =
        line.substr(hyphen_pos + 1, line.size() - hyphen_pos - 1);
    connections.emplace_back(computer_a, computer_b);
  }

  return connections;
}

Network create_network_from_connections(const Connections &connections) {
  Network network;

  ConnectedGroup all_computers;

  for (const auto &[computer_a, computer_b] : connections) {
    all_computers.insert(computer_a);
    all_computers.insert(computer_b);
    network.m_edges[computer_a].insert(computer_b);
    network.m_edges[computer_b].insert(computer_a);
  }

  network.m_computers = Computers(all_computers.begin(), all_computers.end());

  return network;
}

ThreeSets find_all_three_sets(const Network &network) {
  ThreeSets three_sets;
  for (const auto &first_computer : network.m_computers) {
    for (const auto &second_computer : network.m_edges.at(first_computer)) {
      for (const auto &third_computer : network.m_edges.at(second_computer)) {
        if (network.m_edges.at(first_computer).count(third_computer) == 0) {
          continue;
        }
        Computers three_computers{first_computer, second_computer,
                                  third_computer};
        std::sort(three_computers.begin(), three_computers.end());

        three_sets.insert(std::make_tuple(
            three_computers[0], three_computers[1], three_computers[2]));
      }
    }
  }

  return three_sets;
}

ThreeSets find_all_three_sets_starts_with(const Network &network,
                                          const std::string &prefix) {
  const ThreeSets all_three_sets = find_all_three_sets(network);

  ThreeSets three_sets_starts_with;
  for (const auto &three_set : all_three_sets) {
    const auto &[computer_a, computer_b, computer_c] = three_set;
    bool does_start_with = (computer_a.find(prefix) == 0) ||
                           (computer_b.find(prefix) == 0) ||
                           (computer_c.find(prefix) == 0);
    if (does_start_with) {
      three_sets_starts_with.insert(three_set);
    }
  }
  return three_sets_starts_with;
}

std::string part_1(const std::string &filepath) {

  const auto connections = get_connections_from_file(filepath);

  const auto network = create_network_from_connections(connections);

  const auto three_sets = find_all_three_sets_starts_with(network, "t");

  return std::to_string(three_sets.size());
}

std::string part_2(const std::string &filepath) { return std::string(); }

// clang-format off
} // namespace d23
// clang-format on
