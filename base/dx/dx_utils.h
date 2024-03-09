#pragma once

#include <chrono>

namespace base {
namespace dx {

class FpsCounter {
public:
  void Inc() { _count++; };
  bool Update();
  float Fps() { return _fps; };

private:
  float _fps = 0.0f;
  int _count = 0;
  std::chrono::high_resolution_clock::time_point _lastUpdate = std::chrono::high_resolution_clock::now();
  std::chrono::high_resolution_clock::duration _durationUpdate;
};

}
}