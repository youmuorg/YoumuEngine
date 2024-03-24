#include "pix_utils.h"
#include "d3d12_device.h"

namespace base::dx {

void EnableWinPixEventRuntime() {
  typedef void(WINAPI * BeginEventOnCommandList)(
      ID3D12GraphicsCommandList * commandList, UINT64 color,
      _In_ PCSTR formatString);
  typedef void(WINAPI * EndEventOnCommandList)(ID3D12GraphicsCommandList *
                                               commandList);
  typedef void(WINAPI * SetMarkerOnCommandList)(
      ID3D12GraphicsCommandList * commandList, UINT64 color,
      _In_ PCSTR formatString);

  HMODULE module = LoadLibraryW(L"WinPixEventRuntime.dll");

  BeginEventOnCommandList pixBeginEventOnCommandList =
      (BeginEventOnCommandList)GetProcAddress(module,
                                              "PIXBeginEventOnCommandList");
  EndEventOnCommandList pixEndEventOnCommandList =
      (EndEventOnCommandList)GetProcAddress(module, "PIXEndEventOnCommandList");
  SetMarkerOnCommandList pixSetMarkerOnCommandList =
      (SetMarkerOnCommandList)GetProcAddress(module,
                                             "PIXSetMarkerOnCommandList");
}
}  // namespace base::dx