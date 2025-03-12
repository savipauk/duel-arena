#pragma once

#include <SDL_net.h>

#include <iostream>

#include "msgpack.hpp"

#define DARENA_PORT 50325
#define DARENA_MAX_MESSAGE_LENGTH 1024
#define DARENA_CONNECTION_AWAIT 250

#define ISLAND_X_OFFSET 80
#define ISLAND_Y_OFFSET 320
#define ISLAND_HEIGHT 100
#define ISLAND_WIDTH 322
#define ISLAND_POINT_EVERY 7
#define ISLAND_NUM_OF_POINTS (ISLAND_WIDTH / ISLAND_POINT_EVERY)

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
  Position(float x, float y = 0) : x(x), y(y) {}

  std::string to_string() const;
};

// Point on an island with position relative to the island and a height value.
// Used in the island heightmap.
struct IslandPoint {
  int height;

  IslandPoint(int height) : height(height) {}

  std::string to_string() const;
};

struct TCPMessage {
  std::string id;
  std::string data;

  TCPMessage() {}
  TCPMessage(std::string id, std::string data) : id(id), data(data) {}

  MSGPACK_DEFINE(id, data);
};

std::string ipaddress_to_string(IPaddress* address);
std::string unit32_t_address_to_string(uint32_t address);

// Globals

extern Logger log;

}  // namespace darena
