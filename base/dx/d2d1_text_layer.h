#pragma once

#include "d2d1_layer.h"
#include "dwrite_utils.h"
#include "dx_utils.h"

namespace base {
namespace dx {

class D2d1DebugInfoLayer : public D2d1Layer {
public:
  D2d1DebugInfoLayer(ID2D1RenderTarget* rt, IDWriteFactory* wf);
  virtual ~D2d1DebugInfoLayer() {}

public:
  virtual void Draw(ID2D1RenderTarget* rt) override;

private:
  ComPtr<ID2D1SolidColorBrush> _brush;
  ComPtr<IDWriteTextFormat> _format;
  FpsCounter _fpsCounter;
};

}
}
