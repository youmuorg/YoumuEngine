#pragma once

#include "d2d1_base.h"
#include "d2d1_compositor.h"

#include <memory>
#include <list>

namespace base {
namespace win {
namespace dx {

class D2d1Layer : public D2d1LayerNode {
public:
  D2d1Layer(ID2D1RenderTarget* rt);
  virtual ~D2d1Layer() {};

protected:
  ComPtr<ID2D1Layer> _layer;
};

class D2d1QuadLayer : public D2d1Layer {
public:
  D2d1QuadLayer(ID2D1RenderTarget* rt);
  virtual ~D2d1QuadLayer() {}

public:
  virtual void Draw(ID2D1RenderTarget* rt) override;

private:
  ComPtr<ID2D1SolidColorBrush> _brush;
};

}
}
}
