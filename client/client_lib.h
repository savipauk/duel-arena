#include "common.h"

namespace darena {

// Island class used in the client. Defines the island position and height map.
class Island {
 public:
  HeightMap height_map;
  Island ();
  Island (HeightMap height_map) : height_map(height_map) {}
  Island (Position position, int height) : height_map(position, height) {}

  void print() {
    darena::log << "Island" << height_map << "\n";
  }
};

}  // namespace darena
