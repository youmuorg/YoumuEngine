#pragma once

#include "dx_common.h"

#include <directx/d3dcommon.h>

namespace base {
namespace dx {

class HlslCompiler {
public:
  static void CompileVertexShader(const wchar_t* filePath, const char* entrypoint, ID3DBlob** blob);
  static void CompilePixelShader(const wchar_t* filePath, const char* entrypoint, ID3DBlob** blob);
};

}
}