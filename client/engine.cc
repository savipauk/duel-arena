#include "engine.h"

#include <SDL_opengl.h>
#include <SDL_video.h>

#include "client_lib.h"
#include "common.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "imgui_stdlib.h"

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

void draw_islands() {
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
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard |
                    ImGuiConfigFlags_NavEnableSetMousePos;

  io.FontGlobalScale = 1.5f;

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
    ImGui_ImplSDL2_ProcessEvent(&e);
    switch (e.type) {
      case SDL_QUIT:
        game_running = false;
        break;
      case SDL_KEYDOWN:
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
  switch (game->connection) {
    case INITIAL: {
      break;
    }
    case CONNECTING: {
      bool successfully_connected = game->connect_to_server();
      if (successfully_connected) {
        game->connection = CONNECTED;
      }
      break;
    }
    case CONNECTED: {
      break;
    }
    case DISCONNECTED: {
      break;
    }
    default: {
      // Nothing
    }
  }

  darena::log << "Connection: " << std::to_string(game->connection) << "\n";
}

bool Engine::render() {
  // Background
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Make (0,0) top left
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  switch (game->connection) {
    case INITIAL: {
      // Render username control in the middle
      ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
      ImVec2 window_pos =
          ImVec2(viewport_size.x * 0.5f, viewport_size.y * 0.5f);
      ImVec2 window_size = ImVec2(WINDOW_WIDTH * 0.35f, WINDOW_HEIGHT * 0.25f);
      ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
      ImGui::SetNextWindowSize(window_size);

      ImGui::Begin("INPUT", nullptr,
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

      ImGui::InputText("Username", &game->username);
      ImGui::InputText("Server IP", &game->server_ip);
      bool button = ImGui::Button("Connect");

      if (button) {
        game->connection = CONNECTING;
      }

      ImGui::End();
      break;
    }
    case CONNECTING: {
      break;
    }
    case CONNECTED: {
      draw_islands();
      break;
    }
    case DISCONNECTED: {
      break;
    }
    default: {
      // Nothing
    }
  }

  // Error checking
  // TODO: Consider returning here
  GLenum err = glGetError();
  switch (err) {
    case GL_NO_ERROR:
      return true;

    case GL_INVALID_OPERATION:
    case GL_OUT_OF_MEMORY:
      darena::log << "OpenGL Error: " << std::to_string(err) << "\n";
      darena::log << "Critical OpenGL Error. Stopping rendering.\n";
      return false;

    default:
      darena::log << "OpenGL Error: " << std::to_string(err) << "\n";
      return true;
  }
}

bool Engine::run() {
  bool noerr = true;

  noerr = initialize();
  if (!noerr) {
    return false;
  }
  game_running = true;
  setup_game();
  bool show_demo_window = true;

  while (game_running) {
    process_input();

    ImGuiIO& io = ImGui::GetIO();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    update();
    noerr = render();
    if (!noerr) {
      return false;
    }

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
