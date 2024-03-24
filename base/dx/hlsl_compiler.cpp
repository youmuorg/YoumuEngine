#include "hlsl_compiler.h"

#include <d3dcompiler.h>

#include <mutex>

namespace base::dx {

static HMODULE _GetD3dCompilerLib() {
  static std::once_flag loadFlag;
  static HMODULE lib = nullptr;

  std::call_once(loadFlag,
                 []() { lib = ::LoadLibraryW(L"d3dcompiler_47.dll"); });
  _ThrowIfError("LoadLibraryW", lib != nullptr);
  return lib;
}

using D3DCompileFromFileProc = HRESULT(WINAPI)(LPCWSTR pFileName,
                                               CONST D3D_SHADER_MACRO* pDefines,
                                               ID3DInclude* pInclude,
                                               LPCSTR pEntrypoint,
                                               LPCSTR pTarget,
                                               UINT Flags1,
                                               UINT Flags2,
                                               ID3DBlob** ppCode,
                                               ID3DBlob** ppErrorMsgs);

static D3DCompileFromFileProc* _GetD3DCompileFromFileFun() {
  static D3DCompileFromFileProc* d3DCompileFromFile = nullptr;
  static std::once_flag flag;

  std::call_once(flag, []() {
    d3DCompileFromFile = reinterpret_cast<D3DCompileFromFileProc*>(
        ::GetProcAddress(_GetD3dCompilerLib(), "D3DCompileFromFile"));
    _ThrowIfError("GetProcAddress", d3DCompileFromFile != nullptr);
  });
  _ThrowIfNull(d3DCompileFromFile);
  return d3DCompileFromFile;
}

#if defined(_DEBUG)
UINT kCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
UINT kCompileFlags = 0;
#endif

void HlslCompiler::CompileVertexShader(const wchar_t* filePath,
                                       const char* entrypoint,
                                       ID3DBlob** blob) {
  HRESULT hr =
      _GetD3DCompileFromFileFun()(filePath, nullptr, nullptr, entrypoint,
                                  "vs_5_0", kCompileFlags, 0, blob, nullptr);
  _ThrowIfFailed(hr);
}

void HlslCompiler::CompilePixelShader(const wchar_t* filePath,
                                      const char* entrypoint,
                                      ID3DBlob** blob) {
  HRESULT hr =
      _GetD3DCompileFromFileFun()(filePath, nullptr, nullptr, entrypoint,
                                  "ps_5_0", kCompileFlags, 0, blob, nullptr);
  _ThrowIfFailed(hr);
}

void HlslCompiler::CompileShader(const wchar_t* filePath,
                                 const char* entrypoint,
                                 const char* target,
                                 ID3DBlob** blob) {
  HRESULT hr =
      _GetD3DCompileFromFileFun()(filePath, nullptr, nullptr, entrypoint,
                                  target, kCompileFlags, 0, blob, nullptr);
  _ThrowIfFailed(hr);
}

}  // namespace base::dx
