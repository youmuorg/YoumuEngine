#pragma once

#include "../com_error.h"

#include <dxgi1_6.h>
#include <wrl.h>

namespace base {
namespace win {
namespace dx {

using Microsoft::WRL::ComPtr;
using namespace win;

class DxgiFactory {
public:
  DxgiFactory();

  IDXGIFactory4* factory() { return _factory.Get(); }

private:
  ComPtr<IDXGIFactory4> _factory;
};

class DxgiDevice {
public:
  // 从窗口创建交换链
  DxgiDevice(IUnknown* d3dDevice, HWND windowHandle);

  void Clear();
  void Present();

  IDXGIAdapter* adapter() { return _adapter.Get(); }
  IDXGISwapChain* swapchain() { return _swapchain.Get(); }

private:
  ComPtr<IDXGIDevice> _device;
  ComPtr<IDXGIAdapter> _adapter;
  ComPtr<IDXGIFactory2> _factory;
  ComPtr<IDXGISwapChain1> _swapchain;
};

class DxgiDuplication {
public:
  DxgiDuplication(IUnknown* d3dDevice, IDXGIAdapter* adapter);

  void AcquireFrame();
  void ReleaseFrame();

  IDXGIResource* frame() { return _frame.Get(); }

private:
  ComPtr<IDXGIOutputDuplication> _duplication;
  ComPtr<IDXGIResource> _frame;
};

} // namespace dx
} // namespace win
} // namespace base
