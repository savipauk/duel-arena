#pragma once

#include <vector>

#include "common.h"

namespace darena {

struct GameMaster {
  std::vector<darena::IslandPoint> generate_heightmap(
      const Vec2& starting_position, int num_of_points);
};

}  // namespace darena
