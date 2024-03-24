#include "d3d12_cube.h"
#include "d3d12_pipeline.h"
#include "hlsl_compiler.h"


#include <DirectXMath.h>
#include <DirectXColors.h>
#include <directx/d3dx12.h>

namespace base::dx {

namespace Colors {
  using namespace DirectX::Colors;
}

void D3d12Cube::CreatePipelineState(ID3D12Device* device,
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
}

void D3d12Cube::CreateBoxGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {
  D3d12SimpleColorInputLayout::Vertex vertices[] = {
    {{-1.0f, -1.0f, -1.0f}, XMFLOAT4(Colors::White)},
    {{-1.0f, +1.0f, -1.0f}, XMFLOAT4(Colors::Black)},
    {{+1.0f, +1.0f, -1.0f}, XMFLOAT4(Colors::Red)},
    {{+1.0f, -1.0f, -1.0f}, XMFLOAT4(Colors::Green)},
    {{-1.0f, -1.0f, +1.0f}, XMFLOAT4(Colors::Blue)},
    {{-1.0f, +1.0f, +1.0f}, XMFLOAT4(Colors::Yellow)},
    {{+1.0f, +1.0f, +1.0f}, XMFLOAT4(Colors::Cyan)},
    {{+1.0f, -1.0f, +1.0f}, XMFLOAT4(Colors::Magenta)},
  };
  this->CreateVertexBufferFromMemory(device, commandList, vertices, sizeof(vertices), sizeof(D3d12SimpleColorInputLayout::Vertex));

  uint16_t indices[] = {
    // 前
    0, 1, 2,
    0, 2, 3,
    // 后
    4, 6, 5, 
    4, 7, 6, 
    // 左
    4, 5, 1, 
    4, 1, 0, 
    // 右
    3, 2, 6, 
    3, 6, 7, 
    // 上
    1, 5, 6, 
    1, 6, 2,
    // 下
    4, 0, 3, 
    4, 3, 7
  };
  this->CreateIndexBufferFromMemory(device, commandList, indices, sizeof(indices), _countof(indices), DXGI_FORMAT_R16_UINT);
}

void D3d12Cube::Draw(ID3D12GraphicsCommandList* commandList) {
  commandList->SetPipelineState(_pipelineState.Get());

  // 绑定 IA 状态
  D3D12_VERTEX_BUFFER_VIEW vertexBuffers[] = { this->vertexBufferView() };
  commandList->IASetVertexBuffers(0, 1, vertexBuffers);
  D3D12_INDEX_BUFFER_VIEW indexBuffers[] = { this->indexBufferView() };
  commandList->IASetIndexBuffer(indexBuffers);
  commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // commandList->DrawIndexedInstanced(this->indexCount(), 1, 0, 0, 0);
  commandList->DrawInstanced(8, 1, 0, 0);
}


}  // namespace base::dx