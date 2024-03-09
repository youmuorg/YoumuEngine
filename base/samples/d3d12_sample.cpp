#include <base/win/com_common.h>
#include <base/dx/d3d12_utils.h>
#include <base/dx/d3d12_window.h>
#include <base/dx/dxgi_utils.h>
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
    dx::D3d12EnableDebugLayer();
    dx::D3d12Window wnd;

    wnd.Show();
    wnd.Paint();

    win::MessageLoop mq;
    mq.Run();
  } catch (win::ComError& error) {
    std::cout << "[" << error.sourceLocation().file_name() << "("
              << error.sourceLocation().line() << ":"
              << error.sourceLocation().column() << ")] " << error.what()
              << std::endl;
  }

  return 0;
}