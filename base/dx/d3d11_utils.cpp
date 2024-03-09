#include "d3d11_utils.h"
#include <base/win/com_error.h>

#include <directxmath.h>
#include <d3dcompiler.h>

#include <mutex>

using namespace base::win;

namespace base {
namespace dx {

D3d11Device::D3d11Device() {
  // 创建硬件加速设备
  D3D_DRIVER_TYPE driverTypes[] = {
    D3D_DRIVER_TYPE_HARDWARE,
    D3D_DRIVER_TYPE_WARP,
    D3D_DRIVER_TYPE_REFERENCE,
  };
  UINT numDriverTypes = 
      sizeof(driverTypes) / sizeof(driverTypes[0]);

  D3D_FEATURE_LEVEL featureLevels[] = {
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_1,
  };
  UINT numFeatureLevels = 
      sizeof(featureLevels) / sizeof(featureLevels[0]);

  D3D_FEATURE_LEVEL selectedLevel;

  using D3d11CreateDeviceProc = HRESULT (WINAPI) (
      IDXGIAdapter* pAdapter,
      D3D_DRIVER_TYPE DriverType,
      HMODULE Software,
      UINT Flags,
      CONST D3D_FEATURE_LEVEL* pFeatureLevels,
      UINT FeatureLevels,
      UINT SDKVersion,
      ID3D11Device** ppDevice,
      D3D_FEATURE_LEVEL* pFeatureLevel,
      ID3D11DeviceContext** ppImmediateContext);
  static D3d11CreateDeviceProc* createDevice = nullptr;
  static std::once_flag loadFlag;
  std::call_once(loadFlag, []() -> void {
    HMODULE libD3d11 = ::LoadLibraryW(L"d3d11.dll");
    _ThrowIfError("LoadLibraryW", libD3d11 != NULL);

    createDevice = 
        reinterpret_cast<D3d11CreateDeviceProc*>(
            GetProcAddress(libD3d11, "D3D11CreateDevice"));
    _ThrowIfError("GetProcAddress", createDevice != nullptr);
  });

  HRESULT hr = createDevice(
      nullptr, 
      D3D_DRIVER_TYPE_HARDWARE, 
      nullptr, 
      D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT, 
      featureLevels, 
      numFeatureLevels, 
      D3D11_SDK_VERSION, 
      _device.GetAddressOf(), 
      &selectedLevel, 
      _context.GetAddressOf());
  _ThrowIfFailed(hr);
  _ThrowIfNot(selectedLevel >= D3D_FEATURE_LEVEL_11_0);
}

void D3d11Device::Clear() {

}

D3d11RenderTarget::D3d11RenderTarget(ID3D11Device* dev, UINT width, UINT height) {
  D3D11_TEXTURE2D_DESC desc;
  desc.ArraySize = 1;
  desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
  desc.CPUAccessFlags = 0;
  desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  desc.Width = width;
  desc.Height = height;
  desc.MipLevels = 1;
  desc.MiscFlags = 0;
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;
  desc.Usage = D3D11_USAGE_DEFAULT;

  HRESULT hr = dev->CreateTexture2D(&desc, nullptr, &_buffer);
  _ThrowIfFailed(hr);

  // 创建 RTV 资源。
  hr = dev->CreateRenderTargetView(_buffer.Get(), nullptr, _view.GetAddressOf());
  _ThrowIfFailed(hr);

  InitResource(dev);
}

D3d11RenderTarget::D3d11RenderTarget(ID3D11Device* dev, IDXGISwapChain1* swapChain1) {
  CreateRtv(dev, swapChain1);
  InitResource(dev);
}

void D3d11RenderTarget::CreateRtv(ID3D11Device* dev, IDXGISwapChain1* swapChain1) {
  // 取得交换链后台缓冲。
  HRESULT hr = swapChain1->GetBuffer(0, IID_PPV_ARGS(&_buffer));
  _ThrowIfFailed(hr);

  D3D11_TEXTURE2D_DESC desc = {0};
  _buffer->GetDesc(&desc);
  _ThrowIfNot((desc.BindFlags & D3D11_BIND_RENDER_TARGET) != 0);

  // 创建 RTV 资源。
  hr = dev->CreateRenderTargetView(_buffer.Get(), nullptr, _view.GetAddressOf());
  _ThrowIfFailed(hr);

  // 设置视口
  _viewport.Width = static_cast<float>(desc.Width);
  _viewport.Height = static_cast<float>(desc.Height);
  _viewport.MinDepth = D3D11_MIN_DEPTH;
  _viewport.MaxDepth = D3D11_MAX_DEPTH;
  _viewport.TopLeftX = 0;
  _viewport.TopLeftY = 0;
}

void D3d11RenderTarget::InitResource(ID3D11Device* dev) {
  HRESULT hr = S_OK;

  // 创建采样器
  {
    D3D11_SAMPLER_DESC desc = {};
    desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    desc.MinLOD = 0.0f;
    desc.MaxLOD = D3D11_FLOAT32_MAX;
    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    dev->CreateSamplerState(&desc, _sampler.GetAddressOf());
  }

  // 创建混合器
  {
    D3D11_BLEND_DESC desc;
    desc.AlphaToCoverageEnable = FALSE;
    desc.IndependentBlendEnable = FALSE;
    const auto count = sizeof(desc.RenderTarget) / sizeof(desc.RenderTarget[0]);
    for (size_t n = 0; n < count; ++n) {
      desc.RenderTarget[n].BlendEnable = TRUE;
      desc.RenderTarget[n].SrcBlend = D3D11_BLEND_ONE;
      desc.RenderTarget[n].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
      desc.RenderTarget[n].SrcBlendAlpha = D3D11_BLEND_ONE;
      desc.RenderTarget[n].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
      desc.RenderTarget[n].BlendOp = D3D11_BLEND_OP_ADD;
      desc.RenderTarget[n].BlendOpAlpha = D3D11_BLEND_OP_ADD;
      desc.RenderTarget[n].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    }
    dev->CreateBlendState(&desc, _blender.GetAddressOf());
  }
}

void D3d11RenderTarget::Bind(ID3D11DeviceContext* ctx) {
  ctx->RSSetViewports(1, &_viewport);

  ID3D11RenderTargetView* rtv[1] = {_view.Get()};
  ctx->OMSetRenderTargets(1, rtv, nullptr);

  float factor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  ctx->OMSetBlendState(_blender.Get(), factor, 0xffffffff);

  ID3D11SamplerState* samplers[1] = {_sampler.Get()};
  ctx->PSSetSamplers(0, 1, samplers);
}

void D3d11RenderTarget::Unbind(ID3D11DeviceContext* ctx) {
  ctx->OMSetRenderTargets(0, nullptr, nullptr);
}

void D3d11RenderTarget::ReleaseRtv(ID3D11DeviceContext* ctx) {
  _view.Reset();
  _buffer.Reset();
}

void D3d11RenderTarget::Clear(ID3D11DeviceContext* ctx, float red, float green, float blue, float alpha) {
  FLOAT color[4] = {red, green, blue, alpha};
  ctx->ClearRenderTargetView(_view.Get(), color);
}

D3dShaderCompiler::D3dShaderCompiler() {}

D3dShaderCompiler D3dShaderCompiler::_instance;

void D3dShaderCompiler::Init() {
  // 准备编译着色器
  static std::once_flag loadFlag;
  std::call_once(loadFlag, [this](){
    HMODULE lib = ::LoadLibraryW(L"d3dcompiler_47.dll");
    _ThrowIfError("LoadLibraryW", lib != nullptr);

    _d3dCompile = reinterpret_cast<D3dCompileProc*>(
        ::GetProcAddress(lib, "D3DCompile"));
  });
  _ThrowIfNot(_d3dCompile != nullptr);
}

void D3dShaderCompiler::Compile(
    const std::string& code, 
    const std::string& entry, 
    const std::string& model, 
    ID3DBlob** blob) {
  DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(NDEBUG)
// flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
// flags |= D3DCOMPILE_AVOID_FLOW_CONTROL;
#else
  flags |= D3DCOMPILE_DEBUG;
  flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

  ComPtr<ID3DBlob> blobError;

  HRESULT hr = _d3dCompile(
      code.c_str(), 
      code.length() + 1, 
      nullptr, 
      nullptr, 
      nullptr, 
      entry.c_str(),
      model.c_str(), 
      flags, 
      0, 
      blob,
      blobError.GetAddressOf());
  if (FAILED(hr)) {
    std::string errorMsg(reinterpret_cast<char*>(blobError->GetBufferPointer()), blobError->GetBufferSize());
    _ComThrow(errorMsg);
  }
  _ThrowIfFailed(hr);
}

struct TriangleVertex {
  DirectX::XMFLOAT3 pos;
  DirectX::XMFLOAT4 color;
};

D3d11Triangle::D3d11Triangle(ID3D11Device* dev) {
  HRESULT hr = S_OK; 

  // 顶点着色器
  const std::string vsh = R"--(
    struct VertexIn
    {
      float3 pos : POSITION;
      float4 color : COLOR;
    };

    struct VertexOut
    {
      float4 pos : SV_POSITION;
      float4 color : COLOR;
    };

    VertexOut VS(VertexIn vertexIn)
    {
      VertexOut vertexOut;
      vertexOut.pos = float4(vertexIn.pos, 1.0f);
      vertexOut.color = vertexIn.color;
      return vertexOut;
    }
  )--";
  const std::string vertexEntry = "VS";
  const std::string vertexModel = "vs_5_0";

  D3dShaderCompiler::instance().Compile(
      vsh, 
      vertexEntry,
      vertexModel, 
      _vertexBlob.GetAddressOf());

  dev->CreateVertexShader(
      _vertexBlob->GetBufferPointer(),
      _vertexBlob->GetBufferSize(),
      nullptr,
      _vertexShader.GetAddressOf());

  // 输入布局
  D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
        D3D11_INPUT_PER_VERTEX_DATA, 0},
      {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
        D3D11_INPUT_PER_VERTEX_DATA, 0},
  };
  UINT numLayoutDesc = sizeof(layoutDesc) / sizeof(layoutDesc[0]);

  dev->CreateInputLayout(
      layoutDesc, 
      numLayoutDesc,
      _vertexBlob->GetBufferPointer(),
      _vertexBlob->GetBufferSize(), 
      _inputLayout.GetAddressOf());

  // 像素着色
  const std::string psh = R"--(
    struct VertexOut
    {
      float4 pos : SV_POSITION;
      float4 color : COLOR;
    };

    float4 PS(VertexOut pixelIn) : SV_Target
    {
      return pixelIn.color;
    }
  )--";

  const std::string pixelEntry = "PS";
  const std::string pixelModel = "ps_5_0";

  D3dShaderCompiler::instance().Compile(
      psh, 
      pixelEntry,
      pixelModel, 
      _pixelBlob.GetAddressOf());

  dev->CreatePixelShader(
      _pixelBlob->GetBufferPointer(),
      _pixelBlob->GetBufferSize(),
      nullptr,
      _pixelShader.GetAddressOf());

  // 顶点缓冲
  TriangleVertex vertices[] = {
      { DirectX::XMFLOAT3(0.0f, 0.5f, 0.5f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
      { DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
      { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }};

  D3D11_BUFFER_DESC bufferDesc{0};
  bufferDesc.Usage = D3D11_USAGE_DEFAULT;
  bufferDesc.ByteWidth = sizeof(vertices);
  bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  bufferDesc.CPUAccessFlags = 0;

  D3D11_SUBRESOURCE_DATA srd{0};
  srd.pSysMem = vertices;

  hr = dev->CreateBuffer(&bufferDesc, &srd, &_buffer);
  _ThrowIfFailed(hr);
  _stride = sizeof(vertices[0]);
  _vertices = sizeof(vertices) / sizeof(vertices[0]);

  // 图元类型
  _primitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; // 
}

void D3d11Triangle::Bind(ID3D11DeviceContext* ctx) {
  ctx->IASetInputLayout(_inputLayout.Get());
  ctx->VSSetShader(_vertexShader.Get(), nullptr, 0);
  ctx->PSSetShader(_pixelShader.Get(), nullptr, 0);

  // TODO: Handle offset.
  UINT offset = 0;

  ID3D11Buffer* buffers[] = {_buffer.Get()};
  UINT numBuffers = sizeof(buffers) / sizeof(buffers[0]);
  ctx->IASetVertexBuffers(0, numBuffers, buffers, &_stride, &offset);
  ctx->IASetPrimitiveTopology(_primitive);
}

void D3d11Triangle::Unbind(ID3D11DeviceContext* ctx) {

}

void D3d11Triangle::Draw(ID3D11DeviceContext* ctx) {
  ctx->Draw(_vertices, 0);
}

struct QuadVertex {
  DirectX::XMFLOAT3 pos;
  DirectX::XMFLOAT2 tex;
};

D3d11Quad::D3d11Quad(ID3D11Device* dev, UINT x, UINT y, UINT width, UINT height) {
  CreatePixelShaderResource(dev, width, height);
  InitResource(dev, x, y, width, height);
}

D3d11Quad::D3d11Quad(ID3D11Device* dev, ID3D11Texture2D* tex) {
  _pixelShaderResource = tex;

  D3D11_TEXTURE2D_DESC desc;
  _pixelShaderResource->GetDesc(&desc);

  InitResource(dev, 0, 0, desc.Width, desc.Height);
}
void D3d11Quad::CreatePixelShaderResource(ID3D11Device* dev, UINT width, UINT height) {
  // 创建着色器资源
  D3D11_TEXTURE2D_DESC desc;
  desc.ArraySize = 1;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  desc.CPUAccessFlags = 0;
  desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  desc.Width = width;
  desc.Height = height;
  desc.MipLevels = 1;
  desc.MiscFlags = 0;
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;
  desc.Usage = D3D11_USAGE_DEFAULT;

  //D3D11_SUBRESOURCE_DATA srd;
  //srd.pSysMem = data;
  //srd.SysMemPitch = static_cast<UINT>(row_stride);
  //srd.SysMemSlicePitch = 0;

  HRESULT hr = dev->CreateTexture2D(&desc, nullptr, _pixelShaderResource.ReleaseAndGetAddressOf());
  _ThrowIfFailed(hr);
}

void D3d11Quad::InitResource(ID3D11Device* dev, UINT x, UINT y, UINT width, UINT height) {
  HRESULT hr = S_OK;

  D3D11_TEXTURE2D_DESC texDesc;
  _pixelShaderResource->GetDesc(&texDesc);
  _ThrowIfNot((texDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE) != 0);

  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
  srvDesc.Format = texDesc.Format;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MostDetailedMip = 0;
  srvDesc.Texture2D.MipLevels = 1;

  hr = dev->CreateShaderResourceView(_pixelShaderResource.Get(), &srvDesc, _pixelShaderResourceView.GetAddressOf());
  _ThrowIfFailed(hr);

  //_x = (x * 2.0f) - 1.0f;
  //_y = 1.0f - (y * 2.0f);
  //_width = width * 2.0f;
  //_height = height * 2.0f;
  _x = -1.0f;
  _y = 1.0f;
  _width = 2.0f;
  _height = 2.0f;
  _z = 0.5f;

  QuadVertex vertices[] = {
      {DirectX::XMFLOAT3(_x, _y, _z), DirectX::XMFLOAT2(0.0f, 0.0f)},
      {DirectX::XMFLOAT3(_x + _width, _y, _z), DirectX::XMFLOAT2(1.0f, 0.0f)},
      {DirectX::XMFLOAT3(_x, _y - _height, _z), DirectX::XMFLOAT2(0.0f, 1.0f)},
      {DirectX::XMFLOAT3(_x + _width, _y - _height, _z), DirectX::XMFLOAT2(1.0f, 1.0f)}};

  //if (flip) {
  //  DirectX::XMFLOAT2 tmp(vertices[2].tex);
  //  vertices[2].tex = vertices[0].tex;
  //  vertices[0].tex = tmp;

  //  tmp = vertices[3].tex;
  //  vertices[3].tex = vertices[1].tex;
  //  vertices[1].tex = tmp;
  //}

  D3D11_BUFFER_DESC desc = {};
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.ByteWidth = sizeof(QuadVertex) * 4;
  desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  desc.CPUAccessFlags = 0;

  D3D11_SUBRESOURCE_DATA srd = {};
  srd.pSysMem = vertices;

  hr = dev->CreateBuffer(&desc, &srd, &_buffer);
  _ThrowIfFailed(hr);

  _primitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
  _vertices = 4;
  _stride = sizeof(QuadVertex);

  // 顶点着色
  const std::string vsh = R"--(
      struct VS_INPUT
      {
        float4 pos : POSITION;
        float2 tex : TEXCOORD0;
      };

      struct VS_OUTPUT
      {
        float4 pos : SV_POSITION;
        float2 tex : TEXCOORD0;
      };

      VS_OUTPUT main(VS_INPUT input)
      {
        VS_OUTPUT output;
        output.pos = input.pos;
        output.tex = input.tex;
        return output;
      }
    )--";
  const std::string vertexEntry = "main";
  const std::string vertexModel = "vs_4_0";

  D3dShaderCompiler::instance().Compile(
      vsh, 
      vertexEntry,
      vertexModel, 
      _vertexBlob.GetAddressOf());

  dev->CreateVertexShader(
      _vertexBlob->GetBufferPointer(),
      _vertexBlob->GetBufferSize(),
      nullptr,
      _vertexShader.GetAddressOf());

  D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
        D3D11_INPUT_PER_VERTEX_DATA, 0},
      {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
        D3D11_INPUT_PER_VERTEX_DATA, 0},
  };

  UINT numLayoutDesc = sizeof(layoutDesc) / sizeof(layoutDesc[0]);

  dev->CreateInputLayout(
      layoutDesc, 
      numLayoutDesc,
      _vertexBlob->GetBufferPointer(),
      _vertexBlob->GetBufferSize(), 
      _inputLayout.GetAddressOf());

  const std::string psh = R"--(
    Texture2D tex0 : register(t0);
    SamplerState samp0 : register(s0);

    struct VS_OUTPUT
    {
      float4 pos : SV_POSITION;
      float2 tex : TEXCOORD0;
    };

    float4 main(VS_OUTPUT input) : SV_Target
    {
      return tex0.Sample(samp0, input.tex);
    }
  )--";
  const std::string pixelEntry = "main";
  const std::string pixelModel = "ps_4_0";

  D3dShaderCompiler::instance().Compile(
      psh, 
      pixelEntry,
      pixelModel, 
      _pixelBlob.GetAddressOf());

  dev->CreatePixelShader(
      _pixelBlob->GetBufferPointer(),
      _pixelBlob->GetBufferSize(),
      nullptr,
      _pixelShader.GetAddressOf());
}

void D3d11Quad::Bind(ID3D11DeviceContext* ctx) {
  ctx->IASetInputLayout(_inputLayout.Get());
  ctx->VSSetShader(_vertexShader.Get(), nullptr, 0);
  ctx->PSSetShader(_pixelShader.Get(), nullptr, 0);

  ID3D11ShaderResourceView* views[1] = {_pixelShaderResourceView.Get()};
  ctx->PSSetShaderResources(0, 1, views);

  // TODO: Handle offset.
  UINT offset = 0;

  ID3D11Buffer* buffers[1] = {_buffer.Get()};
  ctx->IASetVertexBuffers(0, 1, buffers, &_stride, &offset);
  ctx->IASetPrimitiveTopology(_primitive);
}

void D3d11Quad::Unbind(ID3D11DeviceContext* ctx) {

}

void D3d11Quad::Draw(ID3D11DeviceContext* ctx) {
  ctx->Draw(_vertices, 0);
}

D3d11RenderPipeline::D3d11RenderPipeline() {}

void D3d11RenderPipeline::Draw(ID3D11DeviceContext* ctx) {
}

} // namespace dx
} // namespace base
