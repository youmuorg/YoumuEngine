#pragma once

#include "../com_error.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace base {
namespace win {
namespace dx {

using Microsoft::WRL::ComPtr;

void D3d12EnableDebugLayer();

struct D3d12PipelineConfig {
  UINT frameCount = 2;
};

constexpr D3d12PipelineConfig d3d12PipelineDefaultConfig;

class D3d12Device {
public:
  D3d12Device(IDXGIFactory4* dxgiFactory4);

  ID3D12Device* device() { return _device.Get(); }

private:
  ComPtr<ID3D12Device> _device;

  ComPtr<ID3D12CommandQueue> _commandQueue;
  ComPtr<ID3D12CommandAllocator> _commandAllocator;
  ComPtr<ID3D12CommandList> _commandList;

  ComPtr<ID3D12Fence> _fence;
};

class D3d12Pipeline{
public:
  D3d12Pipeline(ID3D12Device* device, IDXGISwapChain1* swapChain1, const D3d12PipelineConfig& config);

private:
  ComPtr<ID3D12DescriptorHeap> _rtvHeap;
  ComPtr<ID3D12DescriptorHeap> _dsvHeap;
  UINT _rtvDescriptorSize = 0;
  UINT _dsvDescriptorSize = 0;
  UINT _cbvDescriptorSize = 0;
};

}
}
}
