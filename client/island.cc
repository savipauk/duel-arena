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

  bool last_was_zero = false;
  for (size_t i = 0; i < heightmap.size(); ++i) {
    const darena::IslandPoint& point = heightmap[i];
    if (!are_equal(point.position.y,
                   (float)(ISLAND_Y_OFFSET + ISLAND_HEIGHT))) {
      if (last_was_zero) {
        glVertex2i(point.position.x, ISLAND_Y_OFFSET + ISLAND_HEIGHT);
      }
      glVertex2i(point.position.x, point.position.y);
      last_was_zero = false;
      continue;
    }
    if (!last_was_zero && i - 1 > 0) {
      glVertex2i(heightmap[i - 1].position.x, ISLAND_Y_OFFSET + ISLAND_HEIGHT);
    }
    glEnd();
    glBegin(GL_POLYGON);
    last_was_zero = true;
  }

  // Bottom right point last
  glVertex2i(heightmap.back().position.x, ISLAND_Y_OFFSET + ISLAND_HEIGHT);
  glEnd();
}

}  // namespace darena
