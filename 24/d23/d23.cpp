#include <algorithm> // for sort
#include <d23.hpp>
#include <deque>    // for deque
#include <fstream>  // for basic_istream, basic_ifstream, getline, ifstream
#include <iterator> // for next
#include <map>      // for map
#include <numeric>  // for accumulate
#include <set>      // for set, operator!=, __tree_const_iterator
#include <stddef.h> // for size_t
#include <string>   // for allocator, char_traits, string, basic_string
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

ConnectedGroups find_all_cliques(const Network &network) {

  // Want to find every group of computers that is connected to all other
  // computers

  ConnectedGroups cliques;
  ConnectedGroup in_clique;

  for (const auto &computer : network.m_computers) {
    // For each computer that we're connected to, check if they are all
    // connected to one another?
    if (in_clique.count(computer) > 0) {
      // Already found the correct clique
      continue;
    }
    for (const auto &adjacent_computer : network.m_edges.at(computer)) {
      if (in_clique.count(adjacent_computer) > 0) {
        // Already found the correct clique
        continue;
      }
      // Now found at least two computers not already in a clique
      ConnectedGroup current_clique{computer, adjacent_computer};
      std::deque<Computer> computers_to_try;
      for (const auto &clique_computer : current_clique) {
        const auto &adjacent_computers = network.m_edges.at(clique_computer);
        computers_to_try.insert(computers_to_try.end(),
                                adjacent_computers.begin(),
                                adjacent_computers.end());
      }
      while (!computers_to_try.empty()) {
        const auto computer_to_try = computers_to_try.front();
        computers_to_try.pop_front();
        if (current_clique.count(computer_to_try) > 0) {
          continue;
        }
        // Check against all computers in clique
        bool all_connected = true;
        for (const auto &clique_computer : current_clique) {
          all_connected &=
              network.m_edges.at(clique_computer).count(computer_to_try) > 0;
          if (!all_connected) {
            break;
          }
        }
        if (!all_connected) {
          continue;
        }
        current_clique.insert(computer_to_try);
        // Add the adjacent ones to computers_to_try
        const auto &potential_computers = network.m_edges.at(computer_to_try);
        computers_to_try.insert(computers_to_try.end(),
                                potential_computers.begin(),
                                potential_computers.end());
      }
      // Cycle through the clique and add each lookup to
      in_clique.insert(current_clique.begin(), current_clique.end());
      cliques.insert(current_clique);
    }
  }
  return cliques;
}

ConnectedGroup find_largest_clique(const Network &network) {
  const ConnectedGroups all_cliques = find_all_cliques(network);

  size_t largest{};
  ConnectedGroup largest_clique;
  for (const auto &clique : all_cliques) {
    if (clique.size() > largest) {
      largest = clique.size();
      largest_clique = clique;
    }
  }
  return largest_clique;
}

std::string part_1(const std::string &filepath) {

  const auto connections = get_connections_from_file(filepath);

  const auto network = create_network_from_connections(connections);

  const auto three_sets = find_all_three_sets_starts_with(network, "t");

  return std::to_string(three_sets.size());
}

std::string part_2(const std::string &filepath) {

  const auto connections = get_connections_from_file(filepath);

  const auto network = create_network_from_connections(connections);

  const auto largest_clique = find_largest_clique(network);

  Computers clique(largest_clique.begin(), largest_clique.end());
  std::sort(clique.begin(), clique.end());
  const std::string clique_names =
      std::accumulate(std::next(clique.begin()), clique.end(), clique.front(),
                      [](const std::string &left, const std::string &right) {
                        return left + std::string(",") + right;
                      });
  return clique_names;
}

// clang-format off
} // namespace d23
// clang-format on
