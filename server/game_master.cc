#include "game_master.h"

#include <random>

#include "common.h"

namespace darena {

// Used to randomly generate numbers in generate_heightmap()
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

std::vector<darena::IslandPoint> GameMaster::generate_heightmap(
    const Position& starting_position, int num_of_points) {
  std::vector<darena::IslandPoint> output = {};
  int random_strength = 1;
  int last_height = 50 + (dis(gen) - 0.5) * 50;
  float x = starting_position.x;
  float y = starting_position.y;
  for (int i = 0; i < num_of_points; i++) {
    last_height += (dis(gen) - 0.5) * 10;

    if (last_height < 25) {
      last_height = 25;
    } else if (last_height > 75) {
      last_height = 75;
    }

    y = starting_position.y + last_height;
    Position position{x, y};
    output.emplace_back(position, random_strength);
    x += ISLAND_WIDTH / ISLAND_NUM_OF_POINTS;
  }

  return output;
}

}  // namespace darena
