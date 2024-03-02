#include "dxgi_utils.h"
#include "../com_error.h"

#include <mutex>

namespace base {
namespace win {
namespace dx {

using namespace win;

DxgiFactory1::DxgiFactory1(const DxgiCreateParams& params) {
  using CreateDxgiFactory1Proc = HRESULT (WINAPI) (REFIID riid, void **ppFactory);

  static std::once_flag loadFlag;
  static CreateDxgiFactory1Proc* createDxgiFactory1 = nullptr;

  std::call_once(loadFlag, []() -> void {
    HMODULE lib = ::LoadLibraryW(L"dxgi.dll");
    _ApiThrowIfNot("LoadLibraryW", lib != NULL);

    createDxgiFactory1 = 
        reinterpret_cast<CreateDxgiFactory1Proc*>(
            ::GetProcAddress(lib, "CreateDXGIFactory1"));
  });
  _ThrowIfNot(createDxgiFactory1 == nullptr);

  HRESULT hr = createDxgiFactory1(IID_PPV_ARGS(&_factory1));
  _ComThrowIfError(hr);
}

DxgiFactory4::DxgiFactory4() {
  using CreateDxgiFactory2Proc = HRESULT (WINAPI) (UINT flags, REFIID riid, void **ppFactory);

  static std::once_flag loadFlag;
  static CreateDxgiFactory2Proc* createDxgiFactory2 = nullptr;

  std::call_once(loadFlag, []() -> void {
    HMODULE lib = ::LoadLibraryW(L"dxgi.dll");
    _ApiThrowIfNot("LoadLibraryW", lib != NULL);

    createDxgiFactory2 = 
        reinterpret_cast<CreateDxgiFactory2Proc*>(
            ::GetProcAddress(lib, "CreateDXGIFactory2"));
  });
  _ThrowIfNot(createDxgiFactory2 == nullptr);

  UINT flags = 0;
#if defined(_DEBUG)
  flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

  HRESULT hr = createDxgiFactory2(flags, IID_PPV_ARGS(&_factory4));
  _ComThrowIfError(hr);

  _factory4.As(&_factory1);
}

DxgiDevice::DxgiDevice(IUnknown* d3dDevice, HWND windowHandle) {
  HRESULT hr = d3dDevice->QueryInterface(IID_PPV_ARGS(&_device));
  _ComThrowIfError(hr);

  hr = _device->GetAdapter(_adapter.GetAddressOf());
  _ComThrowIfError(hr);

  ComPtr<IDXGIFactory1> factory1;
  hr = _adapter->GetParent(IID_PPV_ARGS(&factory1));
  _ComThrowIfError(hr);

  hr = factory1.As(&_factory2);
  _ComThrowIfError(hr);

  RECT rect;
  memset(&rect, 0, sizeof(rect));
  ::GetClientRect(windowHandle, &rect);
  int width = rect.right - rect.left;
  int height = rect.bottom - rect.top;

  DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {0};
  swapchain_desc.Width = static_cast<UINT>(width);
  swapchain_desc.Height = static_cast<UINT>(height);
  swapchain_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  swapchain_desc.Stereo = FALSE;
  swapchain_desc.SampleDesc.Count = 1;
  swapchain_desc.SampleDesc.Quality = 0;
  swapchain_desc.BufferUsage = 
      DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_BACK_BUFFER;
  swapchain_desc.BufferCount = 2;
  swapchain_desc.Scaling = DXGI_SCALING_STRETCH;
  swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
  swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
  swapchain_desc.Flags = 0;
  swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
  hr = _factory2->CreateSwapChainForHwnd(d3dDevice, windowHandle, 
      &swapchain_desc, nullptr, nullptr, _swapChain1.GetAddressOf());
  _ComThrowIfError(hr);

  // 禁用全屏快捷键
  hr = factory1->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER);
  _ComThrowIfError(hr);
}

void DxgiDevice::Resize(UINT width, UINT height) {
  HRESULT hr = _swapChain1->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
  _ComThrowIfError(hr);
}

void DxgiDevice::Clear() {

}

void DxgiDevice::Present() {
  HRESULT hr = _swapChain1->Present(1, 0);
  _ComThrowIfError(hr);
}

DxgiDuplication::DxgiDuplication(IUnknown* d3dDevice, IDXGIAdapter* adapter) {
  HRESULT hr = S_OK;
  //dx::DxgiFactory dxgiFactory{};
  //dx::ComPtr<IDXGIAdapter1> dxgiAdapter;
  //hr = dxgiFactory.factory1()->EnumAdapters1(0, dxgiAdapter.GetAddressOf());
  //dx::_ComThrowIfError(hr);
  
  //{
  //  DXGI_ADAPTER_DESC desc{0};
  //  _dxgiDevice.adapter()->GetDesc(&desc);
  //  std::wcout << L"Adapter: " << desc.Description << std::endl;
  //}

  dx::ComPtr<IDXGIOutput> dxgiOutput;
  hr = adapter->EnumOutputs(0, &dxgiOutput);
  //hr = dxgiAdapter->EnumOutputs(0, &dxgiOutput);
  dx::_ComThrowIfError(hr);

  dx::ComPtr<IDXGIOutput1> dxgiOutput1;
  hr = dxgiOutput.As(&dxgiOutput1);
  dx::_ComThrowIfError(hr);

  //{
  //  DXGI_OUTPUT_DESC desc{0};
  //  dxgiOutput1->GetDesc(&desc);
  //  std::wcout << L"Output: " << desc.DeviceName << std::endl;
  //}

  hr = dxgiOutput1->DuplicateOutput(d3dDevice, _duplication.GetAddressOf());
  dx::_ComThrowIfError(hr);

  //{
  //  D3D11_TEXTURE2D_DESC desc{0};
  //  acquireFrame->GetDesc(&desc);
  //  std::wcout << L"AcquireFrame: width=" << desc.Width 
  //      << L" height=" << desc.Height
  //      << L" format=" << desc.Format << std::endl;
  //}

}

void DxgiDuplication::AcquireFrame() {
  DXGI_OUTDUPL_FRAME_INFO frameInfo;
  HRESULT hr = _duplication->AcquireNextFrame(500, &frameInfo, _frame.ReleaseAndGetAddressOf());
  if (hr == DXGI_ERROR_ACCESS_LOST || hr == DXGI_ERROR_WAIT_TIMEOUT) {
    // 忽略丢帧
    return;
  }
  dx::_ComThrowIfError(hr);
}

void DxgiDuplication::ReleaseFrame() {
  _duplication->ReleaseFrame();
  _frame.Reset();
}

} // namespace dx
} // namespace win
} // namespace base
