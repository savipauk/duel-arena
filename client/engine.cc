#include "engine.h"

#include "client_lib.h"
#include "common.h"

namespace darena {

std::unique_ptr<darena::Island> left_island;
std::unique_ptr<darena::Island> right_island;

void setup_game() {
  // Create island heightmaps
  left_island = std::make_unique<darena::Island>();
  right_island = std::make_unique<darena::Island>();
  darena::Position left_island_position{ISLAND_X_OFFSET, ISLAND_Y_OFFSET};
  left_island->position =
      std::make_unique<darena::Position>(left_island_position);

  darena::Position right_island_position{
      WINDOW_WIDTH - ISLAND_X_OFFSET - ISLAND_WIDTH, ISLAND_Y_OFFSET};
  right_island->position =
      std::make_unique<darena::Position>(right_island_position);

  std::vector<darena::IslandPoint> left_heightmap =
      darena::create_heightmap(ISLAND_NUM_OF_POINTS);
  left_island->heightmap = left_heightmap;

  std::vector<darena::IslandPoint> right_heightmap =
      darena::create_heightmap(ISLAND_NUM_OF_POINTS);
  right_island->heightmap = right_heightmap;
}

void draw_islands(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  int x = ISLAND_X_OFFSET;
  int y = ISLAND_Y_OFFSET;
  int i = 0;
  SDL_Point* island_points =
      (SDL_Point*)malloc(sizeof(SDL_Point) * ISLAND_NUM_OF_POINTS);
  for (darena::IslandPoint point : left_island->heightmap) {
    int x_increment = ISLAND_WIDTH / ISLAND_NUM_OF_POINTS;
    y = ISLAND_Y_OFFSET + point.height;
    SDL_Point sdl_point{x, y};
    island_points[i] = sdl_point;
    x += x_increment;
    i++;
  }
  SDL_RenderDrawLines(renderer, island_points, ISLAND_NUM_OF_POINTS);

  x = WINDOW_WIDTH - ISLAND_X_OFFSET;
  i = 0;
  for (darena::IslandPoint point : right_island->heightmap) {
    int x_increment = ISLAND_WIDTH / ISLAND_NUM_OF_POINTS;
    y = ISLAND_Y_OFFSET + point.height;
    SDL_Point sdl_point{x, y};
    island_points[i] = sdl_point;
    x -= x_increment;
    i++;
  }
  SDL_RenderDrawLines(renderer, island_points, ISLAND_NUM_OF_POINTS);
}

bool Engine::initialize() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    darena::log << "SDL_Init Error: " << SDL_GetError() << "\n";
    return false;
  }

  window =
      SDL_CreateWindow("Duel Arena", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  if (window == NULL) {
    darena::log << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    darena::log << "SDL_CreateRenderer Error: " << SDL_GetError() << "\n";
    return false;
  }

  return true;
}

void Engine::process_input() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      case SDL_QUIT:
        game_running = false;
        break;
      case SDL_KEYDOWN:
        // Pressed a key
        darena::log << "Pressed a key.\n";
        break;
    }
  }
}

void Engine::update() {
  // Calculate delta time
  float delta_time = (SDL_GetTicks64() - last_frame_time) / 1000.0;
  last_frame_time = SDL_GetTicks64();
  // darena::log << "delta_time: " << delta_time << "\tfps: " << 1.f /
  // delta_time << "\n";

  // Rest of the update function
}

void Engine::render() {
  // Background
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  draw_islands(renderer);

  SDL_RenderPresent(renderer);

  // SDL_Delay(1000 * (1.0 / TARGET_FPS));
}

bool Engine::run() {
  const char* server_ip = "127.0.0.1";
  const char* message = "Hello Server";
  darena::TCPClient client{server_ip, message};

  bool noerr = client.initialize();
  if (!noerr) {
    return false;
  }

  noerr = client.send_connection_request();
  if (!noerr) {
    return false;
  }

  noerr = client.get_connection_response();
  if (!noerr) {
    return false;
  }


  game_running = true;
  setup_game();

  while (game_running) {
    process_input();
    update();
    render();
  }

  client.cleanup();
  return true;
}

void Engine::cleanup() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

}  // namespace darena
