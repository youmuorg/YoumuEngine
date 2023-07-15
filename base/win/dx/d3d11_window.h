#pragma once

#include "../win32_window.h"
#include "d3d11_utils.h"
#include "dxgi1_utils.h"

#include <chrono>
#include <memory>

namespace base {
namespace win {
namespace dx {

class _FpsCounter {
public:
  void Inc() { _count++; };
  bool Update();
  float Fps() { return _fps; };

private:
  float _fps = 0.0f;
  int _count = 0;
  std::chrono::high_resolution_clock::time_point _lastUpdate = std::chrono::high_resolution_clock::now();
  std::chrono::high_resolution_clock::duration _durationUpdate;
};

class D3d11Window : public Win32Window {
public:
  D3d11Window();

  void Capture();
  void Paint();
  void Render();

private:  
  virtual bool OnResize(WPARAM wparam, LPARAM lparam, LRESULT& lresult) override;

private:
  D3d11Device _d3d11Device;
  DxgiDevice _dxgiDevice;
  D3d11RenderTarget _d3d11RenderTarget;
  std::unique_ptr<D3d11Quad> _quad;
  std::unique_ptr<D3d11Triangle> _triangle;
  std::unique_ptr<DxgiDuplication> _duplication;

  _FpsCounter _fpsCounter;
};

} // namespace dx
} // namespace win
} // namespace base
