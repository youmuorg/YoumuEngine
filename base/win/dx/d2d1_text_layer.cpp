#include "d2d1_text_layer.h"

#include <string>

namespace base {
namespace win {
namespace dx {

D2d1DebugInfoLayer::D2d1DebugInfoLayer(ID2D1RenderTarget* rt, IDWriteFactory* dwf) : D2d1Layer(rt) {
  HRESULT hr = rt->CreateSolidColorBrush(
      D2D1::ColorF(D2D1::ColorF::Red),
      &_brush);
  _ComThrowIfError(hr);

  hr = dwf->CreateTextFormat(L"微软雅黑", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 24, L"", &_format);
  _ComThrowIfError(hr);

  hr = _format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
  _ComThrowIfError(hr);
  hr = _format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
  _ComThrowIfError(hr);
}

void D2d1DebugInfoLayer::Draw(ID2D1RenderTarget* rt) {
  _fpsCounter.Inc();
  _fpsCounter.Update();

  rt->SetTransform(D2D1::Matrix3x2F::Identity());
  rt->PushLayer(D2D1::LayerParameters(), _layer.Get());

  std::wstring text(L"FPS: ");
  text += std::to_wstring(static_cast<int>(_fpsCounter.Fps()));
  D2D1_SIZE_F rtSize = rt->GetSize();
  rt->DrawTextW(text.c_str(), text.length(), _format.Get(), D2D1::RectF(0, 0, rtSize.width, rtSize.height), _brush.Get());

  rt->PopLayer();
}

}
}
}