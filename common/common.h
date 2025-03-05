#pragma once

#include <iostream>

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

// Globals

extern Logger log;

}  // namespace darena
