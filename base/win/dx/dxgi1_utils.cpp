#include "dxgi1_utils.h"
#include "../com_error.h"

#include <mutex>

namespace base {
namespace win {
namespace dx {

using namespace win;

DxgiFactory::DxgiFactory() {
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

  IDXGIFactory* fac = nullptr;
  HRESULT hr = createDxgiFactory2(0, __uuidof(IDXGIFactory1), reinterpret_cast<void**>(_factory.GetAddressOf()));
  _ComThrowIfError(hr);
}

DxgiDevice::DxgiDevice(IUnknown* d3dDevice, HWND windowHandle) {
  HRESULT hr = d3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(_device.GetAddressOf()));
  _ComThrowIfError(hr);

  hr = _device->GetAdapter(_adapter.GetAddressOf());
  _ComThrowIfError(hr);

  ComPtr<IDXGIFactory1> factory1;
  hr = _adapter->GetParent(__uuidof(IDXGIFactory1), 
      reinterpret_cast<void**>(factory1.GetAddressOf()));
  _ComThrowIfError(hr);

  hr = factory1.As(&_factory);
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
  hr = _factory->CreateSwapChainForHwnd(d3dDevice, windowHandle, 
      &swapchain_desc, nullptr, nullptr, _swapchain.GetAddressOf());
  _ComThrowIfError(hr);
}

void DxgiDevice::Clear() {

}

void DxgiDevice::Present() {
  _swapchain->Present(1, 0);
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
