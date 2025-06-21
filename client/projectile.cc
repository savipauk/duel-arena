
#include "projectile.h"

#include <SDL_opengl.h>

#include "game.h"

namespace darena {

void Projectile::hit(darena::Game* game) {
  game->projectile_hit();
  velocity_y = 0;
  velocity_x = 0;
  no_hit_frames_count = 0;
}

int Projectile::island_hit_poll(darena::Game* game,
                                std::vector<darena::IslandPoint>& heightmap,
                                size_t check_index, float nose_x,
                                float nose_y) {
  darena::IslandPoint& point = heightmap[check_index];

  // Can't hit terrain that doesn't exist
  if (point.position.y >= (float)(ISLAND_Y_OFFSET + ISLAND_HEIGHT)) {
    return 0;
  }

  if (nose_y >= point.position.y &&
      nose_y <= (float)(ISLAND_Y_OFFSET + ISLAND_HEIGHT) &&
      nose_x >= point.position.x - ISLAND_POINT_EVERY / 2.0f &&
      nose_x <= point.position.x + ISLAND_POINT_EVERY / 2.0f) {
    hit(game);
    // TODO: Update to make use of strength

    int crater_radius = 4;
    float center_impact_modifier = 20.0f;

    for (int i = -crater_radius; i <= crater_radius; ++i) {
      int neighbour_i = (int)(check_index + i);

      if (neighbour_i >= 0 && (size_t)neighbour_i < heightmap.size()) {
        float old_value = heightmap[neighbour_i].position.y;
        float distance_factor =
            1.0f - (std::abs((float)i) / (crater_radius + 1.0f));
        float neighbor_modifier = center_impact_modifier * distance_factor;
        float new_value = old_value + std::max(1.0f, neighbor_modifier);
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

  if (no_hit_frames_count <= max_no_hit_frames) {
    no_hit_frames_count++;
    return;
  }

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
    hit(game);
    game->end_game(!from_a_simulation, Game::GameEndWay::DESTROY);
    return;
  }

  if (game->left_island) {
    auto& left_heightmap = game->left_island->heightmap;
    for (size_t i = 0; i < left_heightmap.size(); ++i) {
      if (island_hit_poll(game, left_heightmap, i, nose_x, nose_y)) {
        game->left_island->rebuild_island_mesh();
        return;
      }
    }
  }

  if (game->right_island) {
    auto& right_heightmap = game->right_island->heightmap;
    for (size_t i = 0; i < right_heightmap.size(); ++i) {
      if (island_hit_poll(game, right_heightmap, i, nose_x, nose_y)) {
        game->right_island->rebuild_island_mesh();
        return;
      }
    }
  }

  if (nose_y >= WINDOW_HEIGHT + height) {
    // Left the screen
    hit(game);
    return;
  }
}

void Projectile::render(darena::Game* game) {
  Vec2 top_left = {-width / 2.0f, height / 2.0f};
  Vec2 top_right = {width / 2.0f, height / 2.0f};
  Vec2 bot_right = {width / 2.0f, -height / 2.0f};
  Vec2 bot_left = {-width / 2.0f, -height / 2.0f};
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
