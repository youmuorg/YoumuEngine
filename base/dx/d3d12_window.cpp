#include "d3d12_window.h"
#include "d2d1_utils.h"

#include <directxtex.h>
#include <wincodec.h>
#include <iostream>
#include <sstream>

namespace base {
namespace dx {

void D3d12Window::Initialize() {
    _dxgiDevice.CreateFactory6();
    _d3d12Device.CreateDevice(_dxgiDevice.factory1());
    _d3d12Device.CreateCommandQueue();

    CreateOverlappedWindow();

    _d3d12Device.CreateSwapchainForHwnd(_dxgiDevice.factory2(), handle());
    _d3d12Device.CreateDescriptorHeaps();
    _d3d12Device.CreateFence();
    _d3d12Device.CreateRtv();

    Render();
    Show();
}

void D3d12Window::OnResize(UINT width, UINT height) {
  if (width == 0 || height == 0) {
    return;
  }

  Render();
}

void D3d12Window::OnMove(INT x, INT y) {
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

void D3d12Window::Render() {
  _d3d12Device.PopulateCommandList();
  _d3d12Device.ExecuteCommandList();
  _d3d12Device.Present();
  _d3d12Device.WaitCommandList();

  // _fpsCounter.Inc();
  // if (_fpsCounter.Update()) {
  //   std::wstringstream ss;
  //   ss.setf(std::ios::fixed);
  //   ss.setf(std::ios::showpoint);
  //   ss.precision(2);
  //   ss << "FPS: " << _fpsCounter.Fps();
  //   SetTitle(ss.str().c_str());
  // }

}

} // namespace dx
} // namespace base
