#pragma once

#include <windows.h>

#include <atomic>

namespace base {
namespace win {

struct Message {
  MSG msg;

};

class MessageQueue {
public:
  bool Dispatch();
  bool IsEmpty();
  bool IsNotEmpty() { return !IsEmpty(); }
};

} // namespace win
} // namespace base
