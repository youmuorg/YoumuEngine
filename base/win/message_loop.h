#pragma once

#include "win32_common.h"

#include <atomic>

namespace base {
namespace win {

struct Message {
  MSG msg;

};

// 窗口线程消息循环
class MessageLoop {
public:
  int Run();
  bool Dispatch();
  bool IsEmpty();
  bool IsNotEmpty() { return !IsEmpty(); }
};

} // namespace win
} // namespace base
