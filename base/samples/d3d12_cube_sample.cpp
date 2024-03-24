

#include <base/dx/d3d12_device.h>
#include <base/dx/d3d12_window.h>
#include <base/dx/d3d12_cube.h>
#include <base/dx/dxgi_utils.h>
#include <base/win/com_common.h>
#include <base/win/win32_window.h>
#include <base/win/win32_application.h>

#include <chrono>
#include <iostream>
#include <thread>

using namespace std::literals::chrono_literals;
using namespace base;

class D3d12SampleWindow : public dx::D3d12Window {
protected:
  virtual void OnInitAssets(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12RootSignature* rootSignature) override {
    _cube.CreatePipelineState(device, commandList, rootSignature);
    _cube.CreateBoxGeometry(device, commandList);
  }
  virtual void OnDraw(ID3D12GraphicsCommandList* commandList) override {
    _cube.Draw(commandList);
  }

private:
  dx::D3d12Cube _cube;
};

class D3d12SampleApp : public win::Win32Application {
public:
  void Init() {
    _window.Initialize();
  }

private:
  D3d12SampleWindow _window;
};

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
  try {
    win::ComApartment::Initialize();

    win::Win32Window::RegisterWindowClass();

    auto app = std::make_unique<D3d12SampleApp>();
    app->Init();
    app->RunMessageLoop();

  } catch (win::ComError& error) {
    std::cout << "[" << error.sourceLocation().file_name() << ":"
              << error.sourceLocation().line() << ","
              << error.sourceLocation().column() << "] " << error.what()
              << std::endl;
  }

  return 0;
}