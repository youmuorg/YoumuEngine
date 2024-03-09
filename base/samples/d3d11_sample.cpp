#include <base/dx/d3d11_utils.h>
#include <base/dx/d3d11_window.h>
#include <base/dx/dxgi_utils.h>
#include <base/win/com_common.h>
#include <base/win/message_loop.h>

#include <chrono>
#include <iostream>
#include <thread>

using namespace std::literals::chrono_literals;
using namespace base;

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
  win::ComApartment::Initialize();
  win::Win32Window::RegisterWindowClass();

  try {
    dx::D3d11Window wnd;

    wnd.Show();
    // wnd.Paint();
    // wnd.Capture();

    win::MessageLoop mq;
    while (1) {
      while (mq.Dispatch()) {
      }
      try {
        wnd.Capture();
        wnd.Render();
        wnd.Present();
      } catch (win::ComError error) {
        std::cout << "[" << error.sourceLocation().file_name() << "("
                  << error.sourceLocation().line() << ","
                  << error.sourceLocation().column() << ")] " << error.what();
      }
    }
  } catch (win::ComError& error) {
    std::cout << "[" << error.sourceLocation().file_name() << "("
              << error.sourceLocation().line() << ","
              << error.sourceLocation().column() << ")] " << error.what();
  }

  return 0;
}