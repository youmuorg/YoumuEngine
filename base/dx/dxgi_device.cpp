#include "dxgi_device.h"

#include <mutex>

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
#if defined(_DEBUG)
  flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

  HRESULT hr = createDxgiFactory2(flags, IID_PPV_ARGS(&_factory6));
  _ThrowIfFailed(hr);

  hr = _factory6.As(&_factory2);
  _ThrowIfFailed(hr);
  hr = _factory6.As(&_factory1);
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

}  // namespace dx
}  // namespace base