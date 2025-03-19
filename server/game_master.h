#pragma once

#include <vector>

#include "common.h"

namespace darena {

struct GameMaster {
  std::vector<darena::IslandPoint> generate_heightmap(
      const Position& starting_position, int num_of_points);
};

}  // namespace darena
