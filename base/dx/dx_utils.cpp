#include "dx_utils.h"

using namespace std::chrono_literals;

namespace base {
namespace dx {

bool FpsCounter::Update() {
  auto now = std::chrono::high_resolution_clock::now();
  auto _durationUpdate = now - _lastUpdate;
  bool needUpdate = _durationUpdate > 1s;
  if (needUpdate) {
    _fps = static_cast<float>(_count) / _durationUpdate.count() * 1e9;
    _count = 0;
    _lastUpdate = now;
  }
  return needUpdate;
}

}
}