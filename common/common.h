#pragma once

#include <SDL_net.h>

#include <iostream>
#include <msgpack/adaptor/define_decl.hpp>

#include "msgpack.hpp"

#define DARENA_PORT 50325
#define DARENA_MAX_MESSAGE_LENGTH 1024
#define DARENA_CONNECTION_AWAIT 250

#define ISLAND_X_OFFSET 80
#define ISLAND_Y_OFFSET 300
#define ISLAND_HEIGHT 130
#define ISLAND_WIDTH 322
#define ISLAND_POINT_EVERY 13
#define ISLAND_NUM_OF_POINTS (ISLAND_WIDTH / (ISLAND_POINT_EVERY * 1.0f))

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

#define MAX_CLIENTS 2

namespace darena {

// Handles log for client/server specific prefixes.
class Logger {
 public:
#ifdef CLIENT
  std::ostream& operator<<(const std::string& message) {
    return std::cout << "CLIENT: " << message;
  }
  std::ostream& operator<<(const char* message) {
    return std::cout << "CLIENT: " << message;
  }
  // Caution with this!
  template <typename T>
  std::ostream& operator<<(const T& value) {
    return std::cout << "CLIENT: " << value;
  }
#elif defined(SERVER)
  std::ostream& operator<<(const std::string& message) {
    return std::cout << "SERVER: " << message;
  }
  std::ostream& operator<<(const char* message) {
    return std::cout << "SERVER: " << message;
  }
  // Caution with this!
  template <typename T>
  std::ostream& operator<<(const T& value) {
    return std::cout << "SERVER: " << value;
  }
#elif defined(COMMON)
  std::ostream& operator<<(const std::string& message) {
    return std::cout << "COMMON: " << message;
  }
  std::ostream& operator<<(const char* message) {
    return std::cout << "COMMON: " << message;
  }
  // Caution with this!
  template <typename T>
  std::ostream& operator<<(const T& value) {
    return std::cout << "COMMON: " << value;
  }
#else
  std::ostream& operator<<(const std::string& message) {
    return std::cout << "NO-DEFINE: " << message;
  }
  std::ostream& operator<<(const char* message) {
    return std::cout << "NO-DEFINE: " << message;
  }
  // Caution with this!
  template <typename T>
  std::ostream& operator<<(const T& value) {
    return std::cout << "NO_DEFINE: " << value;
  }
#endif
};

// Simple 2D vector with x and y coordinates.
struct Position {
  float x;
  float y;

  Position() {}
  Position(float x, float y) : x(x), y(y) {}

  std::string to_string() const;

  MSGPACK_DEFINE(x, y);
};

extern Position left_island_starting_position;
extern Position right_island_starting_position;

// Point on an island with position relative to the island and a height value.
// Used in the island heightmap.
// TODO: Add terrain strength attribute randomizer
struct IslandPoint {
  Position position;
  int strength;

  IslandPoint() : position(0, 0), strength(1) {}
  IslandPoint(Position position, int strength)
      : position(position), strength(strength) {}

  std::string to_string() const;

  MSGPACK_DEFINE(position, strength);
};

struct ClientConnectionRequest {
  std::string player_name;

  ClientConnectionRequest() {}
  ClientConnectionRequest(std::string player_name) : player_name(player_name) {}

  MSGPACK_DEFINE(player_name);
};

struct ServerIDHeightmapsResponse {
  int client_id;
  std::array<std::vector<darena::IslandPoint>, MAX_CLIENTS> heightmaps;

  MSGPACK_DEFINE(client_id, heightmaps);
};

std::string ipaddress_to_string(IPaddress* address);
std::string unit32_t_address_to_string(uint32_t address);
bool are_equal(float x1, float x2, float epsilon = 1e-10);

// Globals

extern Logger log;

}  // namespace darena
