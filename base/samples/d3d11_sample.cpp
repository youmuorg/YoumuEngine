#include <base/win/message_queue.h>
#include <base/win/dx/d3d11_utils.h>
#include <base/win/dx/dxgi_utils.h>
#include <base/win/dx/d3d11_window.h>

#include <windows.h>

#include <iostream>
#include <chrono>
#include <thread>

using namespace std::literals::chrono_literals;
using namespace base;
namespace dx = base::win::dx;

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
  ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);

  try {
    dx::D3d11Window wnd;

    wnd.Show();
    //wnd.Paint();
    //wnd.Capture();

    win::MessageQueue mq;
    while(1) {
      while(mq.Dispatch()){}
      try {
        wnd.Capture();
        wnd.Render();
      } catch (win::ComError error) {
        std::cout 
          << "[" << error.sourceLocation().file_name()
          << "(" << error.sourceLocation().line() << "," << error.sourceLocation().column() << ")] "
          << error.what();
          }
    }
  } catch (win::ComError& error) {
    std::cout 
      << "[" << error.sourceLocation().file_name()
      << "(" << error.sourceLocation().line() << "," << error.sourceLocation().column() << ")] "
      << error.what();
  }

  return 0;
}