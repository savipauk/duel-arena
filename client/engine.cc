#include "engine.h"

#include <SDL_opengl.h>
#include <SDL_video.h>

#include "client_lib.h"
#include "common.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

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

void draw_islands_opengl() {
  glColor3f(1.0f, 1.0f, 1.0f);

  // Draw the left island
  glBegin(GL_LINE_STRIP);
  int x = ISLAND_X_OFFSET;
  for (const darena::IslandPoint& point : left_island->heightmap) {
    int y = ISLAND_Y_OFFSET + point.height;
    glVertex2i(x, y);
    x += ISLAND_WIDTH / ISLAND_NUM_OF_POINTS;
  }
  glEnd();

  // Draw the right island
  x = WINDOW_WIDTH - ISLAND_X_OFFSET;
  glBegin(GL_LINE_STRIP);
  for (const darena::IslandPoint& point : right_island->heightmap) {
    int y = ISLAND_Y_OFFSET + point.height;
    glVertex2i(x, y);
    x -= ISLAND_WIDTH / ISLAND_NUM_OF_POINTS;
  }
  glEnd();
}

bool Engine::initialize() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    darena::log << "SDL_Init Error: " << SDL_GetError() << "\n";
    return false;
  }

  window = SDL_CreateWindow("Duel Arena", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

  if (window == nullptr) {
    darena::log << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
    return false;
  }

  // Initialize OpenGL context
  gl_context = SDL_GL_CreateContext(window);
  if (gl_context == nullptr) {
    darena::log << "SDL_GL_CreateContext Error: " << SDL_GetError() << "\n";
    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }

  SDL_GL_MakeCurrent(window, gl_context);

  // Enable vsync
  SDL_GL_SetSwapInterval(1);

  const char* glsl_version = "#version 130";

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

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
  ImGuiIO& io = ImGui::GetIO();

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  // Properties window
  ImGui::Begin("My Window");
  ImGui::Text("Color");
  ImGui::End();

  // Background
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Make (0,0) top left
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // draw_islands(renderer);
  draw_islands_opengl();

  // Render ImGui
  ImGui::Render();
  glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // Swap the window
  SDL_GL_SwapWindow(window);

  // Frame limiting
  SDL_Delay(1000 * (1.0 / TARGET_FPS));

  // Error checking
  // TODO: Consider returning here
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    darena::log << "OpenGL Error: " << std::to_string(err) << "\n";
  }
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

  noerr = initialize();
  if (!noerr) {
    return false;
  }
  game_running = true;
  setup_game();
  bool show_demo_window = true;

  while (game_running) {
    process_input();
    update();
    render();
  }

  // client.cleanup();
  return true;
}

void Engine::cleanup() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  if (gl_context) {
    SDL_GL_DeleteContext(gl_context);
  }
  if (window) {
    SDL_DestroyWindow(window);
  }
  SDL_Quit();
}

}  // namespace darena
