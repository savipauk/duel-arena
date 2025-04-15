#include "game_state.h"

#include <SDL_opengl.h>

#include "game.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_stdlib.h"

namespace darena {

Position left_pos = {100, 100};
Position right_pos = {WINDOW_WIDTH - 100 - 25,  // - 25 because  of player width
                      100};

void GSInitial::process_input(Game* game, SDL_Event* e) { return; }

void GSInitial::update(Game* game, float delta_time) { return; }

void GSInitial::render(Game* game) {
  // Render username control in the middle
  ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
  ImVec2 window_pos = ImVec2(viewport_size.x * 0.5f, viewport_size.y * 0.5f);
  ImVec2 window_size = ImVec2(WINDOW_WIDTH * 0.35f, WINDOW_HEIGHT * 0.25f);
  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(window_size);

  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoSavedSettings;
  ImGui::Begin("INPUT", nullptr, window_flags);

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

void GSConnecting::render(Game* game) {
  const char* message = "CONNECTING";

  ImVec2 text_size = ImGui::CalcTextSize(message);
  ImVec2 padding = ImVec2(20.0f, 20.0f);

  ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
  ImVec2 window_pos = ImVec2(viewport_size.x * 0.5f, viewport_size.y * 0.5f);
  ImVec2 window_size = ImVec2(text_size.x + padding.x, text_size.y + padding.y);

  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(window_size);

  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

  ImGui::Begin("Connecting Overlay", nullptr, window_flags);
  ImGui::TextUnformatted(message);
  ImGui::End();
}

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
  const char* message = "WAITING FOR GAME TO START";

  ImVec2 text_size = ImGui::CalcTextSize(message);
  ImVec2 padding = ImVec2(20.0f, 20.0f);

  ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
  ImVec2 window_pos = ImVec2(viewport_size.x * 0.5f, viewport_size.y * 0.5f);
  ImVec2 window_size = ImVec2(text_size.x + padding.x, text_size.y + padding.y);

  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(window_size);

  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

  ImGui::Begin("Waiting Overlay", nullptr, window_flags);
  ImGui::TextUnformatted(message);
  ImGui::End();
}

void GSConnected::process_input(Game* game, SDL_Event* e) {}

void GSConnected::update(Game* game, float delta_time) {
  if (game->my_turn) {
    game->set_state(std::make_unique<GSPlayTurn>());
  } else {
    game->set_state(std::make_unique<GSWaitTurn>());
  }
}

void GSConnected::render(Game* game) {}

void GSWaitTurn::process_input(Game* game, SDL_Event* e) {}

void GSWaitTurn::job(Game* game) {
  bool got_turn_data = game->get_turn_data();
  if (got_turn_data) {
    thread_running = false;
    game->my_turn = true;
    darena::log << "Player " << game->id << " going from Wait to Play\n";
    game->set_state(std::make_unique<GSPlayTurn>());
  }
}

void GSWaitTurn::update(Game* game, float delta_time) {
  bool expected = false;
  if (thread_running.compare_exchange_strong(expected, true)) {
    std::thread([this, game]() { this->job(game); }).detach();
  }
}

void GSWaitTurn::render(Game* game) {
  const char* message = "WAITING FOR OTHER PLAYER TO FINISH TURN";

  ImVec2 text_size = ImGui::CalcTextSize(message);
  ImVec2 padding = ImVec2(20.0f, 20.0f);

  ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
  ImVec2 window_pos = ImVec2(viewport_size.x * 0.5f, viewport_size.y * 0.25f);
  ImVec2 window_size = ImVec2(text_size.x + padding.x, text_size.y + padding.y);

  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(window_size);

  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

  ImGui::Begin("Turn Overlay", nullptr, window_flags);
  ImGui::TextUnformatted(message);
  ImGui::End();
}

void GSPlayTurn::process_input(Game* game, SDL_Event* e) {
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

void GSPlayTurn::update(Game* game, float delta_time) {}

void GSPlayTurn::render(Game* game) {
  const char* message = "YOUR TURN";

  ImVec2 text_size = ImGui::CalcTextSize(message);
  ImVec2 padding = ImVec2(20.0f, 20.0f);

  ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
  ImVec2 window_pos = ImVec2(viewport_size.x * 0.5f, viewport_size.y * 0.25f);
  ImVec2 window_size = ImVec2(text_size.x + padding.x, text_size.y + padding.y);

  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(window_size);

  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

  ImGui::Begin("Turn Overlay", nullptr, window_flags);
  ImGui::TextUnformatted(message);
  ImGui::End();
}

}  // namespace darena
