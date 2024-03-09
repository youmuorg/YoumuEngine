

#include <base/dx/d3d12_device.h>
#include <base/dx/d3d12_window.h>
#include <base/dx/dxgi_utils.h>
#include <base/win/com_common.h>
#include <base/win/win32_window.h>
#include <base/win/win32_application.h>

#include <chrono>
#include <iostream>
#include <thread>

using namespace std::literals::chrono_literals;
using namespace base;

class MainApp : public win::Win32Application {
public:
  void Init() {
    _window.Initialize();
  }

private:
  dx::D3d12Window _window;
};

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
  try {
    win::ComApartment::Initialize();

    win::Win32Window::RegisterWindowClass();

    auto app = std::make_unique<MainApp>();
    app->Init();
    app->RunMessageLoop();

  } catch (win::ComError& error) {
    std::cout << "[" << error.sourceLocation().file_name() << "("
              << error.sourceLocation().line() << ":"
              << error.sourceLocation().column() << ")] " << error.what()
              << std::endl;
  }

  return 0;
}