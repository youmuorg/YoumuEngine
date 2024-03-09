#include "win32_application.h"
#include "message_loop.h"

namespace base {
namespace win {

void Win32Application::RunMessageLoop() {
  MessageLoop loop;
  loop.Run();
}

}
}