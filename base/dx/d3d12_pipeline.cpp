#include "d3d12_pipeline.h"

namespace base::dx {

std::wstring _GetAssetsPath() {
  const UINT pathSize = 512;
  WCHAR path[pathSize];
  DWORD size = ::GetModuleFileNameW(nullptr, path, pathSize);
  _ThrowIfError("GetModuleFileNameW", size != 0 && size != pathSize);

  WCHAR* lastSlash = wcsrchr(path, L'\\');
  if (lastSlash) {
    *(lastSlash + 1) = L'\0';
  }

  return std::wstring(path);
}

}