#include "d2d1_utils.h"
#include "d2d1_compositor.h"

#include <mutex>

namespace base {
namespace dx {

D2d1Factory::D2d1Factory() {
  using D2d1CreateFactoryProc = 
    HRESULT (WINAPI) (
        D2D1_FACTORY_TYPE factoryType,
        REFIID riid,
        CONST D2D1_FACTORY_OPTIONS *pFactoryOptions,
        void **ppIFactory);
  static D2d1CreateFactoryProc* createFactory = nullptr;
  static std::once_flag loadFlag;
  std::call_once(loadFlag, []() -> void {
    HMODULE libD2d1 = ::LoadLibraryW(L"d2d1.dll");
    _ThrowIfError("LoadLibraryW", libD2d1 != NULL);

    createFactory = 
        reinterpret_cast<D2d1CreateFactoryProc*>(
            GetProcAddress(libD2d1, "D2D1CreateFactory"));
    _ThrowIfError("GetProcAddress", createFactory != nullptr);
  });

  HRESULT hr = S_OK;
  hr = createFactory(
      D2D1_FACTORY_TYPE_SINGLE_THREADED,
      __uuidof(ID2D1Factory),
      nullptr,
      &_factory);
  _ThrowIfFailed(hr);
}

D2d1Device::D2d1Device(ID2D1Factory1* factory, IDXGIDevice* dxgiDevice) {
  HRESULT hr = factory->CreateDevice(dxgiDevice, &_device);
  _ThrowIfFailed(hr);
  hr = _device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &_context);
  _ThrowIfFailed(hr);
}

void D2d1Device::Clear() {

}

D2d1RenderTarget::D2d1RenderTarget(ID2D1DeviceContext* context) {
  _renderTarget = context;
}

D2d1RenderTarget::D2d1RenderTarget(
    ID2D1Factory* d2d1Factory, 
    IDXGISurface* dxgiSurface) {
  HRESULT hr = S_OK;
  D2D1_RENDER_TARGET_PROPERTIES props =
      D2D1::RenderTargetProperties(
          D2D1_RENDER_TARGET_TYPE_DEFAULT,
          D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
          96,
          96);
  hr = d2d1Factory->CreateDxgiSurfaceRenderTarget(
      dxgiSurface,
      &props,
      &_renderTarget);
  _ThrowIfFailed(hr);
}

void D2d1RenderTarget::Begin() {
  _renderTarget->BeginDraw();
  _renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}

void D2d1RenderTarget::End() {
  HRESULT hr = _renderTarget->EndDraw();
  _ThrowIfFailed(hr);
}

void D2d1RenderTarget::Clear() {
  _renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::LightPink));
}

D2d1Quad::D2d1Quad(ID2D1RenderTarget* rt) {
  HRESULT hr = rt->CreateSolidColorBrush(
      D2D1::ColorF(D2D1::ColorF::Blue),
      &_brush);
  _ThrowIfFailed(hr);
}

void D2d1Quad::Draw(ID2D1RenderTarget* rt) {
  D2D1_RECT_F rect = D2D1::RectF(10.0f, 10.0f, 110.0f, 110.0f);
  rt->DrawRectangle(rect, _brush.Get());
  rt->FillRectangle(rect, _brush.Get());
}

}
}
