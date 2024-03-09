#include "d2d1_layer.h"

namespace base {
namespace dx {

D2d1Layer::D2d1Layer(ID2D1RenderTarget* rt) {
  HRESULT hr = rt->CreateLayer(NULL, &_layer);
  _ComThrowIfError(hr);
}

D2d1QuadLayer::D2d1QuadLayer(ID2D1RenderTarget* rt) : D2d1Layer(rt) {
  HRESULT hr = rt->CreateSolidColorBrush(
      D2D1::ColorF(D2D1::ColorF::Blue),
      &_brush);
  _ComThrowIfError(hr);
}

void D2d1QuadLayer::Draw(ID2D1RenderTarget* rt) {
  rt->SetTransform(D2D1::Matrix3x2F::Identity());
  rt->PushLayer(D2D1::LayerParameters(), _layer.Get());

  D2D1_RECT_F rect = D2D1::RectF(10.0f, 10.0f, 110.0f, 110.0f);
  rt->DrawRectangle(rect, _brush.Get());
  rt->FillRectangle(rect, _brush.Get());

  rt->PopLayer();
}

}
}