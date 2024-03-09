#pragma once

#include <directx/d3d12.h>
#include "dx_common.h"


namespace base {
namespace dx {

class D3d12Triangle {
public:
  void CreateAssets(ID3D12Device* device);
  void PopulateCommandList(ID3D12GraphicsCommandList* commandList);

private:
  // pipeline
  ComPtr<ID3D12RootSignature> _rootSignature;
  ComPtr<ID3D12PipelineState> _pipelineState;

  // resource
  ComPtr<ID3D12Resource> _vertexBuffer;
  D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
};

}  // namespace dx
}  // namespace base