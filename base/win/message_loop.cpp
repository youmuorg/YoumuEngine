#include "message_loop.h"
#include "error.h"

namespace base {
namespace win {

int MessageLoop::Run() {
  MSG msg;
  BOOL bret;
  while ((bret = ::GetMessageW(&msg, NULL, 0, 0)) != 0) {
    if (bret == -1) {
      break;
    }

    if (bret) {
      if (msg.message == WM_QUIT) {
        return static_cast<int>(msg.wParam);
      }

      ::TranslateMessage(&msg);
      ::DispatchMessageW(&msg);
    }
  }
  return 0;
}

bool MessageLoop::Dispatch() {
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

bool MessageLoop::IsEmpty() {
  MSG msg;
  BOOL bret = ::PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
  return !bret;
}

} // namespace win
} // namespace base
