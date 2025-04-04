#include "game_state.h"

#include <SDL_opengl.h>

#include "game.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_stdlib.h"

namespace darena {

Position left_pos = {100, 100};
Position right_pos = {WINDOW_WIDTH - 100 - 25,
                      100};  // - 25 because  of player width

void GSInitial::process_input(Game* game, SDL_Event* e) { return; }

void GSInitial::update(Game* game, float delta_time) { return; }

void GSInitial::render(Game* game) {
  // Render username control in the middle
  ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
  ImVec2 window_pos = ImVec2(viewport_size.x * 0.5f, viewport_size.y * 0.5f);
  ImVec2 window_size = ImVec2(WINDOW_WIDTH * 0.35f, WINDOW_HEIGHT * 0.25f);
  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(window_size);

  ImGui::Begin("INPUT", nullptr,
               ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

  ImGui::InputText("Username", &game->username);
  ImGui::InputText("Server IP", &game->server_ip);
  bool button = ImGui::Button("Connect");

  if (button) {
    game->set_state(std::make_unique<GSConnecting>());
  }

  ImGui::End();
}

void GSConnecting::process_input(Game* game, SDL_Event* e) { return; }

void GSConnecting::job(Game* game) {
  bool successfully_connected = game->connect_to_server();
  if (successfully_connected) {
    thread_running = false;
    game->set_state(std::make_unique<GSWaitingForIslandData>());
  }
}
void GSConnecting::update(Game* game, float delta_time) {
  if (!thread_running) {
    network_thread = std::thread([this, game]() { this->job(game); });
    network_thread.detach();
    thread_running = true;
  }
}

void GSConnecting::render(Game* game) { ImGui::Text("CONNECTING"); }

void GSWaitingForIslandData::process_input(Game* game, SDL_Event* e) { return; }

void GSWaitingForIslandData::job(Game* game) {
  bool successfully_connected = game->get_island_data();
  if (successfully_connected) {
    thread_running = false;
    Position player_pos = left_pos;
    Position enemy_pos = right_pos;
    std::vector<darena::IslandPoint> player_heightmap =
        game->left_island->heightmap;
    std::vector<darena::IslandPoint> enemy_heightmap =
        game->right_island->heightmap;
    if (game->id == 1) {
      player_pos = right_pos;
      player_heightmap = game->right_island->heightmap;
      enemy_pos = left_pos;
      enemy_heightmap = game->left_island->heightmap;
    }
    game->player =
        std::make_unique<darena::Player>(player_pos.x, player_pos.y, 25, 25);
    game->player->heightmap = player_heightmap;
    game->enemy =
        std::make_unique<darena::Enemy>(enemy_pos.x, enemy_pos.y, 25, 25);
    game->enemy->heightmap = enemy_heightmap;
    game->set_state(std::make_unique<GSConnected>());
  }
}

void GSWaitingForIslandData::update(Game* game, float delta_time) {
  if (!thread_running) {
    network_thread = std::thread([this, game]() { this->job(game); });
    network_thread.detach();
    thread_running = true;
  }
}

void GSWaitingForIslandData::render(Game* game) {
  ImGui::Text("WAITING FOR GAME TO START");
}

void GSConnected::process_input(Game* game, SDL_Event* e) {
  switch (e->type) {
    case SDL_KEYDOWN: {
      if (e->key.keysym.sym == SDLK_r) {
        if (game->id == 0) {
          game->player->position = left_pos;
        } else {
          game->player->position = right_pos;
        }
        game->player->shot_power = 0;
        game->player->shot_state = Player::IDLE;
      }
      break;
    }
  }
}

void GSConnected::update(Game* game, float delta_time) {}

void GSConnected::render(Game* game) {}

}  // namespace darena
