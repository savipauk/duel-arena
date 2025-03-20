#pragma once

#include <vector>

#include "common.h"

namespace darena {

struct Game;

// Defines the island position and height map.
class Island {
 public:
  darena::Position position;
  std::vector<darena::IslandPoint> heightmap;

  Island() {}
  Island(darena::Position position, std::vector<darena::IslandPoint> heightmap)
      : position(position), heightmap(heightmap) {}

  void process_input(darena::Game* game, SDL_Event* e);
  void update(darena::Game* game, float delta_time);
  void render(darena::Game* game);

  std::string to_string() const;
};

}  // namespace darena
