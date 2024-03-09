#pragma once

#include <base/win/win32_window.h>
#include <base/dx/dxgi_device.h>
#include <base/dx/d3d12_device.h>

namespace base {
namespace dx {

class D3d12Window : public win::Win32Window {
public:
  void Initialize();
  void Paint();
  void Render();

private:  
  virtual void OnResize(uint32_t width, uint32_t height) override;
  virtual void OnMove(int32_t x, int32_t y) override;

private:
  D3d12Device _d3d12Device;
  DxgiDevice _dxgiDevice;
};

} // namespace dx
} // namespace base
