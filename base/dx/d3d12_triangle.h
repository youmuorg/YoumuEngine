#pragma once

#include "dx_common.h"
#include "d3d12_pipeline_state.h"
#include "d3d12_pipeline.h"


namespace base {
namespace dx {

class D3d12Triangle : protected D3d12GraphicsPipelineState, public D3d12MeshGeometry {
public:
  void CreatePipelineState(ID3D12Device* device,
                           ID3D12GraphicsCommandList* commandList,
                           ID3D12RootSignature* rootSignature);
  void Draw(ID3D12GraphicsCommandList* commandList);
};

}  // namespace dx
}  // namespace base