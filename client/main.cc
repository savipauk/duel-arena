#include <SDL.h>
#include <SDL_net.h>

#include "client_lib.h"

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

#define ISLAND_X_OFFSET 80
#define ISLAND_Y_OFFSET 320
#define ISLAND_HEIGHT 100
#define ISLAND_WIDTH 320
#define ISLAND_POINT_EVERY 8
#define ISLAND_NUM_OF_POINTS (ISLAND_WIDTH / ISLAND_POINT_EVERY)

#define TARGET_FPS 60

darena::Island left_island;
darena::Island right_island;

SDL_Window* window;
SDL_Renderer* renderer;
bool running = false;
uint64_t last_frame_time = 0;
float fps = 0;

void setup_game() {
  // Create island heightmaps
  darena::Position left_island_position{ISLAND_X_OFFSET, ISLAND_Y_OFFSET};
  left_island.position =
      std::make_unique<darena::Position>(left_island_position);

  darena::Position right_island_position{
      WINDOW_WIDTH - ISLAND_X_OFFSET - ISLAND_WIDTH, ISLAND_Y_OFFSET};
  right_island.position =
      std::make_unique<darena::Position>(right_island_position);

  std::vector<darena::IslandPoint> left_heightmap =
      darena::create_heightmap(ISLAND_NUM_OF_POINTS);
  left_island.heightmap = left_heightmap;

  std::vector<darena::IslandPoint> right_heightmap =
      darena::create_heightmap(ISLAND_NUM_OF_POINTS);
  right_island.heightmap = right_heightmap;
}

void draw_islands(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  int x = ISLAND_X_OFFSET;
  int y = ISLAND_Y_OFFSET;
  int i = 0;
  SDL_Point* island_points =
      (SDL_Point*)malloc(sizeof(SDL_Point) * ISLAND_NUM_OF_POINTS);
  for (darena::IslandPoint point : left_island.heightmap) {
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
  for (darena::IslandPoint point : right_island.heightmap) {
    int x_increment = ISLAND_WIDTH / ISLAND_NUM_OF_POINTS;
    y = ISLAND_Y_OFFSET + point.height;
    SDL_Point sdl_point{x, y};
    island_points[i] = sdl_point;
    x -= x_increment;
    i++;
  }
  SDL_RenderDrawLines(renderer, island_points, ISLAND_NUM_OF_POINTS);
}

bool initialize() {
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

void update() {
  // Calculate delta time
  float delta_time = (SDL_GetTicks64() - last_frame_time) / 1000.0;
  last_frame_time = SDL_GetTicks64();
  // darena::log << "delta_time: " << delta_time << "\tfps: " << 1.f /
  // delta_time << "\n";

  // Rest of the update function
}

void process_input() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_KEYDOWN:
        // Pressed a key
        darena::log << "Pressed a key.\n";
        break;
    }
  }
}

void render() {
  // Background
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  // setup_game();
  draw_islands(renderer);

  SDL_RenderPresent(renderer);

  // SDL_Delay(1000 * (1.0 / TARGET_FPS));
}

void destroy() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

const char* server_ip = "127.0.0.1";
const char* message = "Hello Server";
IPaddress ip;
TCPsocket tcp_socket;

bool initialize_sdlnet() {
  if (SDLNet_ResolveHost(&ip, server_ip, DARENA_PORT) == -1) {
    darena::log << "SDLNet_ResolveHost Error: " << SDLNet_GetError() << "\n";
    return false;
  }
  
  tcp_socket = SDLNet_TCP_Open(&ip);
  if (!tcp_socket) {
    darena::log << "SDLNet_TCP_Open Error: " << SDLNet_GetError() << "\n";
    return false;
  }

  return true;
}

void send_connection_request() {
  int len = strlen(message);
  int result = SDLNet_TCP_Send(tcp_socket, message, len);
  if (result < len) {
    darena::log << "SDLNet_TCP_Send Error: " << SDLNet_GetError() << "\n";
  }
  darena::log << "Sent message to server.\n";
}

int main() {
  running = initialize();
  if (!running) {
    return 1;
  }

  setup_game();
  
  if (!initialize_sdlnet()) {
    return 1;
  }

  send_connection_request();
  SDLNet_Quit();

  while (running) {
    process_input();
    update();
    render();
  }

  destroy();

  return 0;
}
