#pragma once

#include "dxgi_device.h"
#include "d3d12_device.h"

#include <base/win/win32_window.h>

namespace base {
namespace dx {

class D3d12Window : public win::Win32Window {
public:
  void Initialize();
  void Render();

protected:  
  virtual void OnResize(uint32_t width, uint32_t height) override;
  virtual void OnMove(int32_t x, int32_t y) override;

  virtual void OnInitAssets(ID3D12Device* device,
                            ID3D12GraphicsCommandList* commandList,
                            ID3D12RootSignature* rootSignature){};
  virtual void OnDraw(ID3D12GraphicsCommandList* commandList) {};

private:
  D3d12Device _d3d12Device;
  DxgiDevice _dxgiDevice;
};

} // namespace dx
} // namespace base
