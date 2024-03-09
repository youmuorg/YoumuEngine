#pragma once 

#include "d2d1_common.h"


namespace base {
namespace dx {

class D2d1Factory {
public:
  D2d1Factory();

  ID2D1Factory1* factory() { return _factory.Get(); }

private:
  ComPtr<ID2D1Factory1> _factory;
};

class D2d1Device {
public:
  D2d1Device(ID2D1Factory1* factory, IDXGIDevice* dxgiDevice);

  void Clear();

  ID2D1Device* device() { return _device.Get(); }
  ID2D1DeviceContext* context() { return _context.Get(); }

private:
  ComPtr<ID2D1Device> _device;
  ComPtr<ID2D1DeviceContext> _context;
};

class D2d1RenderTarget {
public:
  D2d1RenderTarget(
      ID2D1Factory* d2d1Factory, 
      IDXGISurface* dxgiSurface);
  D2d1RenderTarget(ID2D1DeviceContext* context);

  void Begin();
  void End();
  void Clear();

  ID2D1RenderTarget* renderTarget() const { return _renderTarget.Get(); }

private:
  ComPtr<ID2D1RenderTarget> _renderTarget;
};

class D2d1Quad {
public:
  D2d1Quad(ID2D1RenderTarget* rt);

  void Draw(ID2D1RenderTarget* rt);

private:
  ComPtr<ID2D1SolidColorBrush> _brush;
};


}
}
