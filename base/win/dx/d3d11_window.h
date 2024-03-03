#pragma once

#include "../win32_window.h"
#include "d3d11_utils.h"
#include "dxgi_utils.h"
#include "dx_utils.h"

#include <chrono>
#include <memory>

namespace base {
namespace win {
namespace dx {

class D3d11Window : public Win32Window {
public:
  D3d11Window();

  void Capture();
  void Paint();
  void Render();
  void Present();

protected:  
  virtual void OnResize(UINT width, UINT height) override;
  virtual void OnMove(INT x, INT y) override;

protected:
  std::unique_ptr<D3d11Device> _d3d11Device;
  std::unique_ptr<DxgiDevice> _dxgiDevice;
  std::unique_ptr<D3d11RenderTarget> _d3d11RenderTarget;
  std::unique_ptr<D3d11Quad> _quad;
  std::unique_ptr<D3d11Triangle> _triangle;
  std::unique_ptr<DxgiDuplication> _duplication;

  FpsCounter _fpsCounter;
};

} // namespace dx
} // namespace win
} // namespace base
