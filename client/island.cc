#include "island.h"

#include <SDL_opengl.h>

#include <string>

#include "game.h"

namespace darena {

std::string Island::to_string() const {
  std::string output = "Island(";
  for (auto it = heightmap.begin(); it != heightmap.end(); it++) {
    output.append(it->to_string());
  }
  output.append(")");
  return output;
}

void Island::process_input(Game* game, SDL_Event* e) { return; }

void Island::update(Game* game, float delta_time) { return; }

void Island::render(Game* game) {
  glColor3f(1.0f, 1.0f, 1.0f);

  glBegin(GL_POLYGON);
  // Bottom left point first
  glVertex2i(heightmap[0].position.x, ISLAND_Y_OFFSET + ISLAND_HEIGHT);
  for (const darena::IslandPoint& point : heightmap) {
    glVertex2i(point.position.x, point.position.y);
  }
  // Bottom right point last
  glVertex2i(heightmap.back().position.x, ISLAND_Y_OFFSET + ISLAND_HEIGHT);
  glEnd();

  // Draw the right island
  // glBegin(GL_POLYGON);
  // // Bottom left point first
  // glVertex2i(heightmap[0].position.x, ISLAND_Y_OFFSET + ISLAND_HEIGHT);
  // for (const darena::IslandPoint& point : right_island->heightmap) {
  //   glVertex2i(point.position.x, point.position.y);
  // }
  // // Bottom right point last
  // glVertex2i(right_island->heightmap.back().position.x,
  //            ISLAND_Y_OFFSET + ISLAND_HEIGHT);
  // glEnd();
}

}  // namespace darena
