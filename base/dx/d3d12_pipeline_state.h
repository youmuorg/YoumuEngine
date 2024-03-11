#pragma once

#include "d3d12_device.h"

namespace base {
namespace dx {

// PSO（Pipeline state object）
class D3d12Pso {
public:
  
protected:
  ComPtr<ID3D12PipelineState> _pipelineState;
};

class D3d12GraphicsPso : public D3d12Pso {};

class D3d12ComputePso : public D3d12Pso {};

}  // namespace dx
}  // namespace base