#include "dxgi_device.h"

#include <dxgidebug.h>

#include <mutex>

#pragma comment(lib, "dxguid.lib")

namespace base {
namespace dx {

HMODULE _GetDxgiLib() {
  static std::once_flag loadFlag;
  static HMODULE lib = nullptr;

  std::call_once(loadFlag, []() { lib = ::LoadLibraryW(L"dxgi.dll"); });
  _ThrowIfError("LoadLibraryW", lib != nullptr);
  return lib;
}

void DxgiDevice::CreateFactory6() {
  using CreateDxgiFactory2Proc =
      HRESULT(WINAPI)(UINT flags, REFIID riid, void** ppFactory);

  CreateDxgiFactory2Proc* createDxgiFactory2 =
      reinterpret_cast<CreateDxgiFactory2Proc*>(
          ::GetProcAddress(_GetDxgiLib(), "CreateDXGIFactory2"));
  _ThrowIfNot(createDxgiFactory2 != nullptr);

  UINT flags = 0;
// #if defined(_DEBUG)
//   flags |= DXGI_CREATE_FACTORY_DEBUG;
// #endif

  // 直接创建 IDXGIFacotry6 会导致 VisualStudio 的图形调试器异常
  HRESULT hr = createDxgiFactory2(flags, IID_PPV_ARGS(&_factory2));
  _ThrowIfFailed(hr);

  hr = _factory2.As(&_factory6);
  _ThrowIfFailed(hr);
  hr = _factory2.As(&_factory1);
  _ThrowIfFailed(hr);
}

void DxgiDevice::CreateFactory1() {
  using CreateDxgiFactory1Proc = HRESULT(WINAPI)(REFIID riid, void** ppFactory);

  CreateDxgiFactory1Proc* createDxgiFactory1 =
      reinterpret_cast<CreateDxgiFactory1Proc*>(
          ::GetProcAddress(_GetDxgiLib(), "CreateDXGIFactory1"));
  _ThrowIfNot(createDxgiFactory1 != nullptr);

  HRESULT hr = createDxgiFactory1(IID_PPV_ARGS(&_factory1));
  _ThrowIfFailed(hr);
}

void DxgiDevice::EnableDebug() {
  ComPtr<IDXGIInfoQueue> dxgiInfoQueue;

  using DXGIGetDebugInterface1Proc =
      HRESULT(WINAPI)(UINT Flags, REFIID riid, void** pDebug);

  DXGIGetDebugInterface1Proc* dxgiGetDebugInterface1 = 
    reinterpret_cast<DXGIGetDebugInterface1Proc*>(::GetProcAddress(_GetDxgiLib(), "DXGIGetDebugInterface1"));
  _ThrowIfError("GetProcAddress", dxgiGetDebugInterface1 != nullptr);

  HRESULT hr = dxgiGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue));
  _ThrowIfFailed(hr);

  hr = dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
  _ThrowIfFailed(hr);
  hr = dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
  _ThrowIfFailed(hr);

  DXGI_INFO_QUEUE_MESSAGE_ID hide[] = {80};
  DXGI_INFO_QUEUE_FILTER filter = {};
  filter.DenyList.NumIDs = _countof(hide);
  filter.DenyList.pIDList = hide;
  hr = dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
  _ThrowIfFailed(hr);
}

}  // namespace dx
}  // namespace base