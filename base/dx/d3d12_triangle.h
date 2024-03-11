#pragma once

#include "dx_common.h"
#include "d3d12_pipeline_state.h"


namespace base {
namespace dx {

class D3d12Triangle : protected D3d12GraphicsPso {
public:
  void CreatePipelineState(ID3D12Device* device, ID3D12RootSignature* rootSignature);
  void Draw(ID3D12GraphicsCommandList* commandList);

private:
  // resource
  ComPtr<ID3D12Resource> _vertexBuffer;
  D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
};

}  // namespace dx
}  // namespace base