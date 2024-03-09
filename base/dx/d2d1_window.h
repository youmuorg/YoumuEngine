#pragma once

#include "d2d1_utils.h"
#include "d3d11_window.h"
#include "d2d1_compositor.h"

#include <chrono>
#include <memory>

namespace base {
namespace dx {

class D2d1Window : public D3d11Window {
public:
  D2d1Window();

public:
  void Render();

public:
  const D2d1Compositor& d2d1Compositor() { return _d2d1Compositor; }
  const D2d1RenderTarget* d2d1RenderTarget() { return _d2d1RenderTarget.get(); }

private:  
  void BindResources();
  void UnbindResources();

  virtual void OnResize(UINT width, UINT height) override;
  virtual bool OnPaint(WPARAM wparam, LPARAM lparam, LRESULT& lresult) override;

private:
  D2d1Factory _d2d1Factory;
  D2d1Compositor _d2d1Compositor;

  std::unique_ptr<D2d1RenderTarget> _d2d1RenderTarget;
  std::unique_ptr<D2d1Device> _d2d1Device;
  // std::unique_ptr<D2d1Layer> _d2d1Layer;
  std::unique_ptr<D2d1Quad> _d2d1Quad;
};

} // namespace dx
} // namespace base
