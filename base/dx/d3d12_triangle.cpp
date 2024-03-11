#include "d3d12_triangle.h"
#include "d3d12_device.h"
#include "hlsl_compiler.h"

#include <DirectXMath.h>
#include <directx/d3dx12.h>

#include <string>

using namespace DirectX;

namespace base {
namespace dx {

inline std::wstring GetAssetsPath() {
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

struct Vertex {
  XMFLOAT3 position;
  XMFLOAT4 color;
};

void D3d12Triangle::CreatePipelineState(ID3D12Device* device, ID3D12RootSignature* rootSignature) {
  // 创建着色器
  ComPtr<ID3DBlob> vertexShader;
  ComPtr<ID3DBlob> pixelShader;

  std::wstring assetsPath = GetAssetsPath();
  std::wstring vertexShaderPath = assetsPath + L"shaders.hlsl";
  std::wstring pixelShaderPath = assetsPath + L"shaders.hlsl";

  HlslCompiler::CompileVertexShader(vertexShaderPath.c_str(), "VSMain",
                                    &vertexShader);
  HlslCompiler::CompilePixelShader(pixelShaderPath.c_str(), "PSMain",
                                   &pixelShader);

  // vertex input layout.
  D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
       D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
      {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
       D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};

  // 创建图形管线状态对象（PSO）
  D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
  psoDesc.InputLayout = {inputElementDescs, _countof(inputElementDescs)};
  psoDesc.pRootSignature = rootSignature;
  psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
  psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
  psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
  psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
  psoDesc.DepthStencilState.DepthEnable = FALSE;
  psoDesc.DepthStencilState.StencilEnable = FALSE;
  psoDesc.SampleMask = UINT_MAX;
  psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
  psoDesc.NumRenderTargets = 1;
  psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
  psoDesc.SampleDesc.Count = 1;
  _ThrowIfFailed(device->CreateGraphicsPipelineState(
      &psoDesc, IID_PPV_ARGS(&_pipelineState)));

  // 创建顶点缓冲
  float aspectRatio = 1.0f;
  Vertex triangleVertices[] = {
      {{0.0f, 0.25f * aspectRatio, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
      {{0.25f, -0.25f * aspectRatio, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
      {{-0.25f, -0.25f * aspectRatio, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}};

  const UINT vertexBufferSize = sizeof(triangleVertices);

  // Note: using upload heaps to transfer static data like vert buffers is not
  // recommended. Every time the GPU needs it, the upload heap will be
  // marshalled over. Please read up on Default Heap usage. An upload heap is
  // used here for code simplicity and because there are very few verts to
  // actually transfer.
  CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
  CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
  _ThrowIfFailed(
      device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
                                      &desc, D3D12_RESOURCE_STATE_GENERIC_READ,
                                      nullptr, IID_PPV_ARGS(&_vertexBuffer)));

  // Copy the triangle data to the vertex buffer.
  UINT8* pVertexDataBegin;
  CD3DX12_RANGE readRange(
      0, 0);  // We do not intend to read from this resource on the CPU.
  _ThrowIfFailed(_vertexBuffer->Map(
      0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
  memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
  _vertexBuffer->Unmap(0, nullptr);

  // Initialize the vertex buffer view.
  _vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
  _vertexBufferView.StrideInBytes = sizeof(Vertex);
  _vertexBufferView.SizeInBytes = vertexBufferSize;
}

void D3d12Triangle::Draw(ID3D12GraphicsCommandList* commandList) {
  // Set necessary state.
  commandList->SetPipelineState(_pipelineState.Get());
  commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
  commandList->DrawInstanced(3, 1, 0, 0);
}

}  // namespace dx
}  // namespace base
