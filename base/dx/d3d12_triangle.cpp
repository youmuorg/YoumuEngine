#include "d3d12_triangle.h"
#include "d3d12_device.h"
#include "hlsl_compiler.h"

#include <DirectXMath.h>
#include <directx/d3dx12.h>

#include <string>

using namespace DirectX;

namespace base {
namespace dx {

void D3d12Triangle::CreatePipelineState(ID3D12Device* device,
                                        ID3D12GraphicsCommandList* commandList,
                                        ID3D12RootSignature* rootSignature) {
  // 输入布局
  D3d12SimpleColorInputLayout inputLayout;

  // 顶点着色器
  D3d12Shader vertexShader;
  vertexShader.LoadFromFile(L"simple_color.hlsl", "VSMain", "vs_5_0");

  // 像素着色器
  D3d12Shader pixelShader;
  pixelShader.LoadFromFile(L"simple_color.hlsl", "PSMain", "ps_5_0");

  // 创建图形管线状态对象（PSO）
  D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
  psoDesc.InputLayout = inputLayout.inputLayoutDesc();
  psoDesc.pRootSignature = rootSignature;
  psoDesc.VS = vertexShader.shaderBytecode();
  psoDesc.PS = pixelShader.shaderBytecode();
  psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
  psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
  psoDesc.DepthStencilState.DepthEnable = FALSE;
  psoDesc.DepthStencilState.StencilEnable = FALSE;
  psoDesc.SampleMask = UINT_MAX;
  psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
  psoDesc.NumRenderTargets = 1;
  psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
  psoDesc.SampleDesc.Count = 1;
  HRESULT hr = device->CreateGraphicsPipelineState(
      &psoDesc, IID_PPV_ARGS(&_pipelineState));
  _ThrowIfFailed(hr);

  // 顶点数据
  D3d12SimpleColorInputLayout::Vertex vertices[] = {
      {{0.0f, 0.25f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
      {{0.25f, -0.25f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
      {{-0.25f, -0.25f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}};
  this->CreateVertexBufferFromMemory(
      device, commandList, vertices, sizeof(vertices),
      sizeof(D3d12SimpleColorInputLayout::Vertex));
}

void D3d12Triangle::Draw(ID3D12GraphicsCommandList* commandList) {
  commandList->SetPipelineState(_pipelineState.Get());

  D3D12_VERTEX_BUFFER_VIEW vertexBuffers[] = { this->vertexBufferView() };
  commandList->IASetVertexBuffers(0, 1, vertexBuffers);
  commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  commandList->DrawInstanced(3, 1, 0, 0);
}

}  // namespace dx
}  // namespace base
