#pragma once

#include <SDL_net.h>
#include <iostream>

#define DARENA_PORT 50325
#define DARENA_MAX_MESSAGE_LENGTH 1024
#define DARENA_CONNECTION_AWAIT 250

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
#elif defined(SERVER)
  std::ostream& operator<<(const std::string& message) {
    return std::cout << "SERVER: " << message;
  }
  std::ostream& operator<<(const char* message) {
    return std::cout << "SERVER: " << message;
  }
#else
  std::ostream& operator<<(const std::string& message) {
    return std::cout << message;
  }
  std::ostream& operator<<(const char* message) {
    return std::cout << "NO-DEFINE: " << message;
  }
#endif

  // Caution with this!
  template <typename T>
  std::ostream& operator<<(const T& value) {
    return std::cout << value;
  }
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

struct TCPConnection {
  
};

std::string ipaddress_to_string(IPaddress* address);
std::string unit32_t_address_to_string(uint32_t address);

// Globals

extern Logger log;

}  // namespace darena
