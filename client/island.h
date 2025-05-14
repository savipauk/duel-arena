#pragma once

#include <vector>

#include "common.h"

namespace darena {

struct Game;

// Defines the island position and height map.
class Island {
 private:
  std::vector<std::vector<darena::IslandPoint>> island_vertices;
  std::vector<std::vector<uint>> island_indices;

 public:
  darena::Vec2 position;
  std::vector<darena::IslandPoint> heightmap;

  Island() {}
  Island(darena::Vec2 position, std::vector<darena::IslandPoint> heightmap)
      : position(position), heightmap(heightmap) {}

  void build_island_part(size_t start_i, size_t end_i);
  void rebuild_island_mesh();
  void deprecated_gl_island_render(darena::Game* game);

  void process_input(darena::Game* game, SDL_Event* e);
  void update(darena::Game* game, float delta_time);
  void render(darena::Game* game);

  std::string to_string() const;
};

}  // namespace darena
