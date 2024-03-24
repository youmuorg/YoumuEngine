#pragma once

#include "d3d12_pipeline_state.h"
#include "d3d12_pipeline.h"

namespace base::dx {

// 立方体绘制管线状态
class D3d12Cube : public D3d12GraphicsPipelineState, public D3d12MeshGeometry {
public:
  // 创建 PSO
  void CreatePipelineState(ID3D12Device* device,
                           ID3D12GraphicsCommandList* commandList,
                           ID3D12RootSignature* rootSignature);
  // 创建集合体顶点数据
  void CreateBoxGeometry(ID3D12Device* device,
                         ID3D12GraphicsCommandList* commandList);
  void Draw(ID3D12GraphicsCommandList* commandList);
private:

};

}  // namespace base::dx