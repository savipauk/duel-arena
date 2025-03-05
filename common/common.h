#pragma once

#include <iostream>
#include <memory>

namespace darena {

// Handles log for client/server specific prefixes.
class Logger {
 public:
#ifdef CLIENT
  std::ostream& operator<<(const std::string& message) {
    return std::cout << "CLIENT: " << message;
  }
#elif defined(SERVER)
  std::ostream& operator<<(const std::string& message) {
    return std::cout << "SERVER: " << message;
  }
#else
  std::ostream& operator<<(const std::string& message) {
    return std::cout << message;
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
  int x;
  int y;

  Position(int x = 0, int y = 0) : x(x), y(y) {}

  std::string to_string() const;
};

// Point on an island with position relative to the island and a height value.
// Used in the island heightmap.
struct IslandPoint {
  Position position;
  int height;

  IslandPoint(Position position, int height)
      : position(position), height(height) {}

  std::string to_string() const;
};

// Globals

extern Logger log;

}  // namespace darena
