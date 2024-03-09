#pragma once

#include <base/win/win32_window.h>
#include "d3d12_utils.h"
#include "dxgi_utils.h"
#include "dx_utils.h"

#include <chrono>
#include <memory>

namespace base {
namespace dx {

class D3d12Window : public Win32Window {
public:
  D3d12Window();

  void Paint();
  void Render();

private:  
  virtual void OnResize(UINT width, UINT height) override;
  virtual void OnMove(INT x, INT y) override;

private:
  std::unique_ptr<DxgiFactory4> _dxgiFactory4;
  std::unique_ptr<D3d12Device> _device;
  std::unique_ptr<DxgiDevice> _dxgiDevice;
  std::unique_ptr<D3d12Pipeline> _pipeline;

  // 交换链缓冲索引
  uint32_t _frameIndex = 0;
  //std::unique_ptr<D3d11Quad> _quad;
  //std::unique_ptr<D3d11Triangle> _triangle;
  std::unique_ptr<DxgiDuplication> _duplication;

  FpsCounter _fpsCounter;
};

} // namespace dx
} // namespace base
