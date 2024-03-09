#include "d2d1_window.h"

namespace base {
namespace dx {

D2d1Window::D2d1Window() {
  _d2d1Device = std::make_unique<D2d1Device>(_d2d1Factory.factory(), _dxgiDevice->device());

  BindResources();
  // _d2d1RenderTarget = std::make_unique<D2d1RenderTarget>(_d2d1Factory->factory(), buffer.Get());
  
  // // 至少保证一个D2D资源持有，避免resize时整个D2D设备被重建，导致性能问题
  // _d2d1Layer = std::make_unique<D2d1Layer>(_d2d1RenderTarget->renderTarget());
  _d2d1Quad = std::make_unique<D2d1Quad>(_d2d1RenderTarget->renderTarget());
}

void D2d1Window::BindResources() {
  ComPtr<IDXGISurface> dxgiSurface;
  HRESULT hr = _dxgiDevice->swapChain1()->GetBuffer(0, IID_PPV_ARGS(&dxgiSurface));
  _ThrowIfFailed(hr);

  D2D1_BITMAP_PROPERTIES1 bitmapProps = D2D1::BitmapProperties1(
      D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
      D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
      );

  ComPtr<ID2D1Bitmap1> bitmap;
  hr = _d2d1Device->context()->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), bitmapProps, &bitmap);
  _ThrowIfFailed(hr);

  _d2d1Device->context()->SetTarget(bitmap.Get());
  _d2d1RenderTarget = std::make_unique<D2d1RenderTarget>(_d2d1Device->context());
}

void D2d1Window::UnbindResources() {
  _d2d1RenderTarget.release();
  _d2d1Device->context()->SetTarget(nullptr);
}

void D2d1Window::OnResize(UINT width, UINT height) {
  UnbindResources();

  D3d11Window::OnResize(width, height);

  ComPtr<IDXGISurface> buffer;
  HRESULT hr = _dxgiDevice->swapChain1()->GetBuffer(0, IID_PPV_ARGS(&buffer));
  _ThrowIfFailed(hr);

  BindResources();
}

bool D2d1Window::OnPaint(WPARAM wparam, LPARAM lparam, LRESULT& lresult) {
  D3d11Window::Render();
  Render();
  Present();

  return false;
}

void D2d1Window::Render() {
  if (!_d2d1RenderTarget) return;

  _d2d1RenderTarget->Begin();

  // _d2d1Quad->Draw(_d2d1RenderTarget->renderTarget());
  _d2d1Compositor.Draw(_d2d1RenderTarget->renderTarget());

  _d2d1RenderTarget->End();
}

} // namespace dx
} // namespace base
