#include <vector>

#include "common.h"

namespace darena {

// Defines the island position and height map.
struct Island {
  darena::Position position;
  std::vector<IslandPoint> height_map;
  Island();
  Island(darena::Position position, std::vector<IslandPoint> height_map)
      : position(position), height_map(height_map) {}

  std::string to_string() const;
};

}  // namespace darena
