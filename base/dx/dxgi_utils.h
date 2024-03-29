#pragma once

#include <base/win/com_error.h>

#include <dxgi1_6.h>
#include <wrl.h>

namespace base {
namespace dx {

using Microsoft::WRL::ComPtr;
using namespace win;

struct DxgiCreateParams {};
constexpr DxgiCreateParams dxgiCreate;

class DxgiFactory1 {
public:
  DxgiFactory1(const DxgiCreateParams& params);

protected:
  DxgiFactory1() = default;

public:
  IDXGIFactory1* factory1() { return _factory1.Get(); }

protected:
  ComPtr<IDXGIFactory1> _factory1;
};

class DxgiFactory4 : public DxgiFactory1 {
public:
  DxgiFactory4();

  IDXGIFactory4* factory4() { return _factory4.Get(); }

private:
  ComPtr<IDXGIFactory4> _factory4;
};

class DxgiDeviceUtils {
public:
  // 从窗口创建交换链
  DxgiDeviceUtils(IUnknown* d3dDevice, HWND windowHandle);
  DxgiDeviceUtils(IDXGIFactory2* factory2, IUnknown* d3dDevice, HWND windowHandle);

  void Resize(UINT width, UINT height);
  void Clear();
  void Present();

  IDXGIDevice* device() { return _device.Get(); }
  IDXGIAdapter* adapter() { return _adapter.Get(); }
  IDXGISwapChain1* swapChain1() { return _swapChain1.Get(); }
  IDXGISwapChain3* swapChain3() { return _swapChain3.Get(); }

private:
  ComPtr<IDXGIDevice> _device;
  ComPtr<IDXGIAdapter> _adapter;
  ComPtr<IDXGISwapChain1> _swapChain1;
  ComPtr<IDXGISwapChain3> _swapChain3;
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

}  // namespace dx
} // namespace base
