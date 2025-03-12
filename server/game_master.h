#pragma once

#include <vector>

#include "common.h"

namespace darena {

struct GameMaster {
  std::vector<darena::IslandPoint> generate_heightmap(int num_of_points);
};

}  // namespace darena
