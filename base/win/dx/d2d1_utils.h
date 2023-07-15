#pragma once 

#include <wrl.h>
#include <d2d1_2.h>


namespace base {
namespace win {
namespace dx {

using Microsoft::WRL::ComPtr;

class D2d1Factory {
public:
  D2d1Factory();

  ID2D1Factory* factory() { return _factory.Get(); }

private:
  ComPtr<ID2D1Factory> _factory;
};

class D2d1RenderTarget {
public:
  D2d1RenderTarget(
      ID2D1Factory* d2d1Factory, 
      IDXGISurface* dxgiSurface);

  void Begin();
  void End();
  void Clear();

  ID2D1RenderTarget* renderTarget() { return _renderTarget.Get(); }

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
}
