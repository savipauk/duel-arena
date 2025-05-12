
#include "projectile.h"

#include <SDL_opengl.h>

#include "game.h"

namespace darena {

int Projectile::island_hit_poll(darena::Game* game,
                                std::vector<darena::IslandPoint>& heightmap,
                                size_t check_index, float nose_x,
                                float nose_y) {
  darena::IslandPoint& point = heightmap[check_index];

  if (nose_y >= point.position.y &&
      nose_x >= point.position.x - ISLAND_POINT_EVERY / 2.0f &&
      nose_x <= point.position.x + ISLAND_POINT_EVERY / 2.0f) {
    game->projectile_hit();
    velocity_x = 0;
    velocity_y = 0;

    // TODO: Update to make use of strength

    int crater_radius = 5;
    float center_impact_modifier = 15.0f;

    for (int i = -crater_radius; i <= crater_radius; ++i) {
      int neighbour_i = (int)(check_index + i);

      if (neighbour_i >= 0 && (size_t)i < heightmap.size()) {
        float distance_factor =
            1.0f - (std::abs((float)i) / (crater_radius + 1.0f));
        float neighbor_modifier = center_impact_modifier * distance_factor;
        float new_value = heightmap[neighbour_i].position.y +
                          std::max(1.0f, neighbor_modifier);
        heightmap[neighbour_i].position.y =
            std::min((float)(ISLAND_Y_OFFSET + ISLAND_HEIGHT), new_value);
      }
    }

    return 1;
  }

  return 0;
}

void Projectile::process_input(darena::Game* game, SDL_Event* e) {}

void Projectile::update(darena::Game* game, float delta_time) {
  velocity_y -= gravity;

  position.x += velocity_x * FIXED_TIMESTEP;
  position.y -= velocity_y * FIXED_TIMESTEP;
  angle = std::atan(velocity_y / velocity_x);

  float check_x = game->enemy->position.x;
  float check_w = game->enemy->width;
  float check_y = game->enemy->position.y;
  float check_h = game->enemy->height;

  if (!game->my_turn) {
    check_x = game->player->position.x;
    check_w = game->player->width;
    check_y = game->player->position.y;
    check_h = game->player->height;
  }

  float nose_x = position.x + std::cos(angle) * width / 2.0f * shot_direction;
  float nose_y = position.y + std::sin(angle) * width / 2.0f;
  if (nose_x >= check_x - check_w / 2.0f - width / 2.0f &&
      nose_x <= check_x + check_w / 2.0f + width / 2.0f &&
      nose_y >= check_y - check_h / 2.0f - height / 2.0f &&
      nose_y <= check_y + check_h / 2.0f + height / 2.0f) {
    // Hit enemy
    game->projectile_hit();
    velocity_x = 0;
    velocity_y = 0;
    return;
  }

  if (game->left_island) {
    auto& left_heightmap = game->left_island->heightmap;
    for (size_t i = 0; i < left_heightmap.size(); ++i) {
      if (island_hit_poll(game, left_heightmap, i, nose_x, nose_y)) {
        return;
      }
    }
  }

  if (game->right_island) {
    auto& right_heightmap = game->right_island->heightmap;
    for (size_t i = 0; i < right_heightmap.size(); ++i) {
      if (island_hit_poll(game, right_heightmap, i, nose_x, nose_y)) {
        return;
      }
    }
  }

  if (nose_y >= WINDOW_HEIGHT + height) {
    // Left the screen
    game->projectile_hit();
    velocity_x = 0;
    velocity_y = 0;
    return;
  }
}

void Projectile::render(darena::Game* game) {
  Position top_left = {-width / 2.0f, height / 2.0f};
  Position top_right = {width / 2.0f, height / 2.0f};
  Position bot_right = {width / 2.0f, -height / 2.0f};
  Position bot_left = {-width / 2.0f, -height / 2.0f};
  int angle_deg = angle * (180.0f / M_PI);

  glPushMatrix();
  glTranslatef(position.x, position.y, 0);
  glRotatef(-angle_deg, 0, 0, 1);

  glColor3f(0.3f, 0.75f, 0.3f);
  // if (game->id == 0) {
  //   glColor3f(0.3f, 0.75f, 0.3f);
  // } else {
  //   glColor3f(1.0f, 0.5f, 0.3f);
  // }

  glBegin(GL_POLYGON);

  glVertex2f(top_left.x, top_left.y);
  glVertex2f(top_right.x, top_right.y);
  glVertex2f(bot_right.x, bot_right.y);
  glVertex2f(bot_left.x, bot_left.y);

  glEnd();
  glPopMatrix();
}

}  // namespace darena
