#include <SDL2/SDL.h>

#include "client_lib.h"

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

#define ISLAND_X_OFFSET 80
#define ISLAND_Y_OFFSET 320
#define ISLAND_HEIGHT 100
#define ISLAND_WIDTH 320
#define ISLAND_POINT_EVERY 8
#define ISLAND_NUM_OF_POINTS (ISLAND_WIDTH / ISLAND_POINT_EVERY)

darena::Island left_island;
darena::Island right_island;

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

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("SDL_Init Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window* window =
      SDL_CreateWindow("Duel Arena", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  if (window == NULL) {
    printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Renderer* renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    return 1;
  }

  // Draw background
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  setup_game();

  draw_islands(renderer);

  SDL_RenderPresent(renderer);

  SDL_Event e;
  bool running = true;
  while (running) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        running = false;
        break;
      }
    }
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
