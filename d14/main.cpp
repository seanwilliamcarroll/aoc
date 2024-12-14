#include <fstream>   // for basic_ostream, operator<<, endl, basic_istream
#include <iostream>  // for cout, cerr
#include <stddef.h>  // for size_t
#include <stdexcept> // for runtime_error
#include <string>    // for char_traits, stoll, string
#include <utility>   // for make_pair, pair
#include <vector>    // for vector

using Coordinate = long long;

using Position = std::pair<Coordinate, Coordinate>;

using Velocity = Position;

struct Robot {
  Position m_Position;
  Velocity m_Velocity;
};

using Robots = std::vector<Robot>;

constexpr Coordinate GRID_HEIGHT = 103;
constexpr Coordinate GRID_WIDTH = 101;

void parse_robot(const std::string &line, Robot &robot) {
  size_t pos = line.find_first_of("p=");
  if (pos == std::string::npos) {
    throw std::runtime_error("Unexpected end of line!");
  }
  // Skip over "p="
  pos += 2;
  size_t comma = line.find_first_of(",", pos);
  if (comma == std::string::npos) {
    throw std::runtime_error("Unexpected end of line!");
  }
  Coordinate x_coordinate = std::stoll(line.substr(pos, comma - pos));
  pos = comma + 1;
  size_t v_pos = line.find_first_of(" v=", pos);
  if (v_pos == std::string::npos) {
    throw std::runtime_error("Unexpected end of line!");
  }
  Coordinate y_coordinate = std::stoll(line.substr(pos, v_pos - pos));
  robot.m_Position = std::make_pair(x_coordinate, y_coordinate);

  // Skip over " v="
  pos = v_pos + 3;
  comma = line.find_first_of(",", pos);
  if (comma == std::string::npos) {
    throw std::runtime_error("Unexpected end of line!");
  }
  Coordinate x_velocity = std::stoll(line.substr(pos, comma - pos));
  pos = comma + 1;
  Coordinate y_velocity = std::stoll(line.substr(pos, line.size() - pos));
  robot.m_Velocity = std::make_pair(x_velocity, y_velocity);
}

Robots get_robots_from_file(const std::string &filepath) {
  std::ifstream in_stream(filepath);

  Robots robots;
  for (std::string line; std::getline(in_stream, line); /*BLANK*/) {
    Robot robot;
    parse_robot(line, robot);
    robots.push_back(robot);
  }

  return robots;
}

void print_robot(const Robot &robot) {
  std::cout << "Position: (" << robot.m_Position.first << ", "
            << robot.m_Position.second << ") ";
  std::cout << "Velocity: (" << robot.m_Velocity.first << ", "
            << robot.m_Velocity.second << ") ";
  std::cout << std::endl;
}

void print_robots(const Robots &robots) {
  for (const auto &robot : robots) {
    print_robot(robot);
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

void simulate_one_robot_one_second(Robot &robot) {
  auto &[x_coordinate, y_coordinate] = robot.m_Position;
  auto &[x_velocity, y_velocity] = robot.m_Velocity;

  x_coordinate += x_velocity;
  x_coordinate = (GRID_WIDTH + x_coordinate) % GRID_WIDTH;

  y_coordinate += y_velocity;
  y_coordinate = (GRID_HEIGHT + y_coordinate) % GRID_HEIGHT;
}

void simulate_one_second(Robots &robots) {
  for (auto &robot : robots) {
    simulate_one_robot_one_second(robot);
  }
}

int calculate_safety(const Robots &robots) {
  int safety_scores[4]{};

  for (const auto &robot : robots) {
    const auto [x_coordinate, y_coordinate] = robot.m_Position;
    if ((x_coordinate < GRID_WIDTH / 2) && (y_coordinate < GRID_HEIGHT / 2)) {
      safety_scores[0] += 1;
    } else if ((x_coordinate < GRID_WIDTH / 2) &&
               (y_coordinate > GRID_HEIGHT / 2)) {
      safety_scores[1] += 1;
    } else if ((x_coordinate > GRID_WIDTH / 2) &&
               (y_coordinate < GRID_HEIGHT / 2)) {
      safety_scores[2] += 1;
    } else if ((x_coordinate > GRID_WIDTH / 2) &&
               (y_coordinate > GRID_HEIGHT / 2)) {
      safety_scores[3] += 1;
    } else if ((x_coordinate == GRID_WIDTH / 2) ||
               (y_coordinate == GRID_HEIGHT / 2)) {
      // nothing
    } else {
      throw std::runtime_error(
          "Unexpected position!: " + std::to_string(x_coordinate) + " " +
          std::to_string(y_coordinate));
    }
  }

  return safety_scores[0] * safety_scores[1] * safety_scores[2] *
         safety_scores[3];
}

void print_grid(const Robots &robots) {
  std::vector<std::string> grid(GRID_HEIGHT, std::string(GRID_WIDTH, ' '));

  for (const auto &robot : robots) {
    const auto [x_coordinate, y_coordinate] = robot.m_Position;
    grid[y_coordinate][x_coordinate] = 'X';
  }

  for (const auto &line : grid) {
    std::cout << line << std::endl;
  }
  std::cout << std::endl;
}

int calculate_safety_after_n_seconds(int n_seconds,
                                     const Robots &input_robots) {
  Robots robots(input_robots);

  for (int second = 0; second < n_seconds; ++second) {
    simulate_one_second(robots);
  }
  return calculate_safety(robots);
}

bool search_grid(const Robots &robots) {
  std::vector<std::string> grid(GRID_HEIGHT, std::string(GRID_WIDTH, ' '));

  for (const auto &robot : robots) {
    const auto [x_coordinate, y_coordinate] = robot.m_Position;
    grid[y_coordinate][x_coordinate] = 'X';
  }

  for (const auto &line : grid) {
    if (line.find("XXXXXXXXXXXXXXXXX") != std::string::npos) {
      std::cout << std::endl;
      print_grid(robots);
      return true;
    }
  }
  return false;
}

int find_easter_egg(const Robots &input_robots) {

  Robots robots(input_robots);

  bool found = search_grid(robots);

  int second = 0;

  std::cout << "Second: 0             \r";

  while (!found) {
    std::cout << "\rSecond: " << second + 1 << "    ";
    simulate_one_second(robots);
    found = search_grid(robots);
    ++second;
  }

  return second;
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    std::cerr << "Must provide filepath!" << std::endl;
    return -1;
  }

  const auto robots = get_robots_from_file(argv[1]);

  int accumulator = calculate_safety_after_n_seconds(100, robots);

  std::cout << "Part 1 safety score: " << accumulator << std::endl;

  accumulator = find_easter_egg(robots);

  std::cout << "Part 2 num seconds: " << accumulator << std::endl;

  return 0;
}
