#pragma once

#include <dxgi1_2.h>
#include <d3d11_1.h>
#include <wrl.h>

#include <string>
#include <cstdint>

namespace base {
namespace win {
namespace dx {

using Microsoft::WRL::ComPtr;

class D3d11Device{
public:
  D3d11Device();

  void Clear();

  ID3D11Device* device() { return _device.Get(); }
  ID3D11DeviceContext* context() { return _context.Get(); }

private:
  ComPtr<ID3D11Device> _device;
  ComPtr<ID3D11DeviceContext> _context;
};

class D3d11RenderTarget {
public:
  // 从交换链后台缓存建立渲染目标
  D3d11RenderTarget(ID3D11Device* dev, IDXGISwapChain* swapchain);

  // 直接创建纹理目标
  D3d11RenderTarget(ID3D11Device* dev, uint32_t width, uint32_t height);

  void Bind(ID3D11DeviceContext* ctx);
  void Unbind(ID3D11DeviceContext* ctx);
  void Clear(ID3D11DeviceContext* ctx, float red, float green, float blue, float alpha);

  ID3D11Texture2D* buffer() { return _buffer.Get(); }

private:
  void InitResource(ID3D11Device* dev);

private:
  ComPtr<ID3D11Texture2D> _buffer;
  ComPtr<ID3D11RenderTargetView> _view;

  D3D11_VIEWPORT _viewport;
  ComPtr<ID3D11SamplerState> _sampler;
  ComPtr<ID3D11BlendState> _blender;
};

class D3dShaderCompiler {
private:
  using D3dCompileProc = HRESULT(WINAPI)(
      LPCVOID, SIZE_T, LPCSTR, const D3D_SHADER_MACRO*, ID3DInclude*, LPCSTR,
      LPCSTR, UINT, UINT, ID3DBlob**, ID3DBlob**);

  D3dShaderCompiler();

public:
  static D3dShaderCompiler& instance() { 
    _instance.Init(); 
    return _instance; 
  }

  void Compile(
      const std::string& code, 
      const std::string& entry, 
      const std::string& model, 
      ID3DBlob** blob);

private:
  void Init();

private:
  static D3dShaderCompiler _instance;

  D3dCompileProc* _d3dCompile = nullptr;
};

// 三角形
class D3d11Triangle {
public:
  D3d11Triangle(ID3D11Device* dev);

  void Bind(ID3D11DeviceContext* ctx);
  void Unbind(ID3D11DeviceContext* ctx);
  void Draw(ID3D11DeviceContext* ctx);

private:
  ComPtr<ID3D11Buffer> _buffer;
  D3D_PRIMITIVE_TOPOLOGY _primitive = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
  uint32_t _vertices = 0;
  uint32_t _stride = 0;

  ComPtr<ID3DBlob> _vertexBlob;
  ComPtr<ID3D11VertexShader> _vertexShader;
  ComPtr<ID3D11InputLayout> _inputLayout;
  ComPtr<ID3DBlob> _pixelBlob;
  ComPtr<ID3D11PixelShader> _pixelShader;
};

// 一个四边形
class D3d11Quad {
public:
  D3d11Quad(ID3D11Device* dev, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
  D3d11Quad(ID3D11Device* dev, ID3D11Texture2D* tex);

  void Bind(ID3D11DeviceContext* ctx);
  void Unbind(ID3D11DeviceContext* ctx);
  void Draw(ID3D11DeviceContext* ctx);


  ID3D11Texture2D* texture() { return _pixelShaderResource.Get(); }

private:
  void InitResource(ID3D11Device* dev, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

private:
  float _x = 0.0f;
  float _y = 0.0f;
  float _z = 0.0f;
  float _width = 0.0f;
  float _height = 0.0f;

  ComPtr<ID3D11Buffer> _buffer;
  D3D_PRIMITIVE_TOPOLOGY _primitive = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
  uint32_t _vertices = 0;
  uint32_t _stride = 0;

  ComPtr<ID3DBlob> _vertexBlob;
  ComPtr<ID3D11VertexShader> _vertexShader;
  ComPtr<ID3D11InputLayout> _inputLayout;
  ComPtr<ID3DBlob> _pixelBlob;
  ComPtr<ID3D11PixelShader> _pixelShader;
  ComPtr<ID3D11Texture2D> _pixelShaderResource;
  ComPtr<ID3D11ShaderResourceView> _pixelShaderResourceView;
};

class D3d11RenderPipeline {
public:
  D3d11RenderPipeline();

  void Draw(ID3D11DeviceContext* ctx);
};

} // namespace dx
} // namespace win
} // namespace base
