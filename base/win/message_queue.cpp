#include "message_queue.h"
#include "error.h"

namespace base {
namespace win {

bool MessageQueue::Dispatch() {
  MSG msg;
  BOOL bret = ::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE);

  if (bret) {
    if (msg.message == WM_QUIT) {
      return false;
    }

    // 翻译虚拟按键消息。
    ::TranslateMessage(&msg);

    // 调度窗口过程。
    ::DispatchMessageW(&msg);
  }

  return bret;
}

bool MessageQueue::IsEmpty() {
  MSG msg;
  BOOL bret = ::PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
  return !bret;
}

} // namespace win
} // namespace base
