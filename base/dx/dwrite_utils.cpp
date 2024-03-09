#include "dwrite_utils.h"

#include <mutex>

namespace base {
namespace dx {

DwriteFactory::DwriteFactory() {
  using DWriteCreateFactoryProc = 
    HRESULT (WINAPI) (
        DWRITE_FACTORY_TYPE factoryType,
        REFIID riid,
        IUnknown **factory);
  static DWriteCreateFactoryProc* createFactory = nullptr;
  static std::once_flag loadFlag;
  std::call_once(loadFlag, []() -> void {
    HMODULE libDwrite = ::LoadLibraryW(L"dwrite.dll");
    _ApiThrowIfNot("LoadLibraryW", libDwrite != NULL);

    createFactory = 
        reinterpret_cast<DWriteCreateFactoryProc*>(
            GetProcAddress(libDwrite, "DWriteCreateFactory"));
    _ApiThrowIfNot("GetProcAddress", createFactory != nullptr);
  });

  HRESULT hr = S_OK;
  hr = createFactory(
      DWRITE_FACTORY_TYPE_SHARED,
      __uuidof(*(_factory.Get())),
      reinterpret_cast<IUnknown**>(_factory.GetAddressOf()));
  _ComThrowIfError(hr);
}

}
}
