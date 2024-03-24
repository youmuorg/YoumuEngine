#pragma once

#include "dx_common.h"

#include <directx/d3dcommon.h>

namespace base::dx {

class HlslCompiler {
public:
  static void CompileVertexShader(const wchar_t* filePath,
                                  const char* entrypoint,
                                  ID3DBlob** blob);
  static void CompilePixelShader(const wchar_t* filePath,
                                 const char* entrypoint,
                                 ID3DBlob** blob);
  static void CompileShader(const wchar_t* filePath,
                            const char* entrypoint,
                            const char* target,
                            ID3DBlob** blob);
};

}  // namespace base::dx