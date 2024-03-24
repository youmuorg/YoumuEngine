#pragma once

#include "d3d12_device.h"

namespace base {
namespace dx {

class D3d12PipelineState {
public:
  
protected:
  ComPtr<ID3D12PipelineState> _pipelineState;
};

class D3d12GraphicsPipelineState : public D3d12PipelineState {};

class D3d12ComputePipelineState : public D3d12PipelineState {};

}  // namespace dx
}  // namespace base