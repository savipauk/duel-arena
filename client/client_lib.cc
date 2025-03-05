#include "client_lib.h"

#include <random>

namespace darena {

std::string Island::to_string() const {
  std::string output = "Island(" + position->to_string();
  for (auto it = heightmap.begin(); it != heightmap.end(); it++) {
    output.append(it->to_string());
  }
  output.append(")");
  return output;
}

// Used to randomly generate numbers in create_heightmap()
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

// TODO: Move this function to the server. The server should generate the height
// maps and send them to the clients
std::vector<IslandPoint> create_heightmap(int num_of_points) {
  std::vector<IslandPoint> output = {};

  int last_height = 50 + (dis(gen) - 0.5) * 50;
  for (int i = 0; i < num_of_points; i++) {
    last_height += (dis(gen) - 0.5) * 10;

    if (last_height < 25) {
      last_height = 25;
    } else if (last_height > 75) {
      last_height = 75;
    }

    output.emplace_back(last_height);
  }

  return output;
}

}  // namespace darena
