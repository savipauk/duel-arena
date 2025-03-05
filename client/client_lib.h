#include <vector>

#include "common.h"

namespace darena {

// Defines the island position and height map.
class Island {
 public:
  std::vector<IslandPoint> height_map;
  Island();
  Island(std::vector<IslandPoint> height_map) : height_map(height_map) {}

  void print() {
    darena::log << "Island";
    for (auto it = height_map.begin(); it != height_map.end(); it++) {
      darena::log << *it << "\n";
    }
  }
};

}  // namespace darena
