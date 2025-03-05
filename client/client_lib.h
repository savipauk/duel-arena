#include <memory>
#include <vector>

#include "common.h"

namespace darena {

// Defines the island position and height map.
struct Island {
  std::unique_ptr<darena::Position> position;
  std::vector<IslandPoint> heightmap;

  Island() {}
  Island(std::unique_ptr<darena::Position> position,
         std::vector<IslandPoint> heightmap)
      : position(std::move(position)), heightmap(heightmap) {}

  std::string to_string() const;
};

std::vector<IslandPoint> create_heightmap(int num_of_points);

}  // namespace darena
