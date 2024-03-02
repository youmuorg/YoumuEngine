#include "d3d12_window.h"
#include "d2d1_utils.h"

#include <directxtex.h>
#include <wincodec.h>
#include <iostream>
#include <sstream>

using namespace std::chrono_literals;

namespace base {
namespace win {
namespace dx {

bool _FpsCounter::Update() {
  auto now = std::chrono::high_resolution_clock::now();
  auto _durationUpdate = now - _lastUpdate;
  bool needUpdate = _durationUpdate > 1s;
  if (needUpdate) {
    _fps = static_cast<float>(_count) / _durationUpdate.count() * 1e9;
    _count = 0;
    _lastUpdate = now;
  }
  return needUpdate;
}

D3d12Window::D3d12Window()
  : Win32Window(overlappedWindow),
    _dxgiFactory4(),
    _device(_dxgiFactory4.factory4()),
    _dxgiDevice(_device.device(), handle()),
    _pipeline(_device.device(), _dxgiDevice.swapChain1(), d3d12PipelineDefaultConfig) {
  //_triangle = std::make_unique<D3d11Triangle>(_d3d11Device.device());
}

void D3d12Window::OnResize(UINT width, UINT height) {
  if (width == 0 || height == 0) {
    return;
  }

  //_pipeline.Unbind(_d3d11Device.context());
  //_pipeline.ReleaseRtv(_d3d11Device.context());
  //_dxgiDevice.Resize(width, height);
  //_pipeline.CreateRtv(_d3d11Device.device(), _dxgiDevice.swapChain1());
  //_pipeline.Bind(_d3d11Device.context());

  //_quad = std::make_unique<D3d11Quad>(
  //    _d3d11Device.device(), 
  //    0, 
  //    0, 
  //    static_cast<uint32_t>(clientWidth()), 
  //    static_cast<uint32_t>(clientHeight()));

  //Paint();
  Capture();
  Render();
}

void D3d12Window::OnMove(INT x, INT y) {
  Capture();
  Render();
}

void D3d12Window::Paint() {
  HRESULT hr = S_OK;

  //dx::D3d11RenderTarget target(_d3d11Device.device(), clientWidth(), clientHeight());
  //dx::ComPtr<IDXGISurface> surface;
  //target.buffer()->QueryInterface(__uuidof(IDXGISurface), &surface);
  //dx::D2d1Factory d2d1Factory;
  //dx::D2d1RenderTarget d2d1RenderTarget(d2d1Factory.factory(), surface.Get());
  //dx::D2d1Quad quad(d2d1RenderTarget.renderTarget());

  //d2d1RenderTarget.Begin();
  //d2d1RenderTarget.Clear();
  //quad.Draw(d2d1RenderTarget.renderTarget());
  //d2d1RenderTarget.End();

  //_quad = std::make_unique<D3d11Quad>(
  //    _d3d11Device.device(),
  //    target.buffer());

  //DirectX::ScratchImage image;
  //hr = DirectX::CaptureTexture(_d3d11Device.device(), _d3d11Device.context(), _quad->texture(), image);
  //dx::_ComThrowIfError(hr);

  //hr = DirectX::SaveToWICFile(
  //    image.GetImages(),
  //    image.GetImageCount(),
  //    DirectX::WIC_FLAGS_NONE,
  //    GUID_ContainerFormatPng, 
  //    L"D:\\Temp\\t.png",
  //    &GUID_WICPixelFormat32bppBGRA);
  //dx::_ComThrowIfError(hr);
}

void D3d12Window::Capture() {
  HRESULT hr = S_OK;

  if (!_duplication) {
    _duplication = std::make_unique<DxgiDuplication>(_device.device(), _dxgiDevice.adapter());
  }

  //if (!_quad) {
  //  _quad = std::make_unique<D3d11Quad>(
  //      _d3d11Device.device(), 
  //      0, 
  //      0, 
  //      static_cast<uint32_t>(clientWidth()), 
  //      static_cast<uint32_t>(clientHeight()));
  //}

  _duplication->AcquireFrame();

  if (_duplication->frame()) {
    dx::ComPtr<ID3D12Resource> acquireFrame;
    hr = _duplication->frame()->QueryInterface(__uuidof(ID3D12Resource), &acquireFrame);
    dx::_ComThrowIfError(hr);

    //DirectX::ScratchImage image;
    //hr = DirectX::CaptureTexture(_d3d11Device.device(), _d3d11Device.context(), frameData.Frame, image);
    //dx::_ComThrowIfError(hr);

    //hr = DirectX::SaveToWICFile(
    //    image.GetImages(),
    //    image.GetImageCount(),
    //    DirectX::WIC_FLAGS_NONE,
    //    GUID_ContainerFormatPng, 
    //    L"D:\\Temp\\t.png",
    //    &GUID_WICPixelFormat32bppBGRA);
    //dx::_ComThrowIfError(hr);

    //D3D11_BOX srcRegion{0};
    //srcRegion.left = 0;
    //srcRegion.top = 0;
    //srcRegion.right = clientWidth();
    //srcRegion.bottom = clientHeight();
    //srcRegion.front = 0;
    //srcRegion.back = 1;
    //_d3d11Device.context()->CopySubresourceRegion(_quad->texture(), 0, 0, 0, 0, acquireFrame.Get(), 0, &srcRegion);
  }

  _duplication->ReleaseFrame();
}

void D3d12Window::Render() {
  //_pipeline.Bind(_device.context());
  //_quad->Bind(_device.context());
  //_triangle->Bind(_d3d11Device.context());

  //_pipeline.Clear(_device.context(), 0.5f, 0.5f, 1.0f, 1.0f);

  //_quad->Draw(_device.context());
  //_triangle->Draw(_d3d11Device.context());

  _dxgiDevice.Present();

  _fpsCounter.Inc();
  if (_fpsCounter.Update()) {
    std::wstringstream ss;
    ss.setf(std::ios::fixed);
    ss.setf(std::ios::showpoint);
    ss.precision(2);
    ss << "FPS: " << _fpsCounter.Fps();
    SetTitle(ss.str().c_str());
  }

}

} // namespace dx
} // namespace win
} // namespace base
