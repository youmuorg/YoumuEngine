#pragma once

#include "dx_common.h"

#include <base/win/synchronization.h>

#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include <dxgi1_6.h>

#include <vector>

namespace base {
namespace dx {

class D3d12Device {
public:
  void CreateDevice(IDXGIFactory1* dxgiFactory);
  // 创建交换链
  void CreateSwapchainForHwnd(IDXGIFactory2* dxgiFactory, HWND hwnd);
  // 创建管线相关资源
  void CreatePipeline();

  // 记录命令列表
  void BeginPopulateCommandList();
  void EndPopulateCommandList();
  void PopulateCommandList();
  // 执行命令列表
  void ExecuteCommandList();
  void Present();
  // 等待命令列表
  void WaitCommandList();

  ID3D12Device* device() { return _device.Get(); }
  ID3D12RootSignature* rootSignature() { return _rootSignature.Get(); }
  ID3D12GraphicsCommandList* commandList() { return _commandList.Get(); }

private:
  void CreateFence();
  void CreateCommandList();
  // 创建资源描述符堆
  void CreateDescriptorHeaps();
  // 创建渲染目标视图
  void CreateRtv();
  // 创建深度、模板视图
  void CreateDsv();
  void CreateRootSignature();

public:
  static void EnableDebugLayer();

private:
  ComPtr<ID3D12Device> _device;

  // command queue
  ComPtr<ID3D12CommandQueue> _commandQueue;
  ComPtr<ID3D12CommandAllocator> _commandAllocator;
  ComPtr<ID3D12GraphicsCommandList> _commandList;

  // fence
  ComPtr<ID3D12Fence> _fence;
  uint64_t _fenceValue = 0;

  // swapchain
  ComPtr<IDXGISwapChain3> _swapchain;
  uint32_t _frameCount = 0;  // 交换链缓冲区数量
  uint32_t _frameIndex = 0;  // 当前交换链缓冲区索引
  win::Event _fenceEvent;

  // descriptor heaps
  ComPtr<ID3D12DescriptorHeap> _rtvHeap;  // 渲染目标视图（render target view）描述符堆
  ComPtr<ID3D12DescriptorHeap> _dsvHeap;  // 深度、模板视图（depth/stencil view）描述符堆
  ComPtr<ID3D12DescriptorHeap> _cbvHeap;  // 常量缓冲区视图（constant buffer view）描述符堆
  ComPtr<ID3D12DescriptorHeap> _srvHeap;  // 着色器资源视图（shader resource view）描述符堆
  ComPtr<ID3D12DescriptorHeap> _uavHeap;  // 无序访问视图（unordered access view）描述符堆
  ComPtr<ID3D12DescriptorHeap> _samplerHeap;  // 采样器（sampleer）描述符堆
  uint32_t _rtvDescriptorSize = 0;
  uint32_t _dsvDescriptorSize = 0;
  uint32_t _cbvDescriptorSize = 0;

  // descriptor
  std::vector<ComPtr<ID3D12Resource>> _rtvBuffers;

  // pipeline
  CD3DX12_VIEWPORT _viewport;
  CD3DX12_RECT _scissorRect;

  // assets
  ComPtr<ID3D12RootSignature> _rootSignature;
};

}  // namespace dx
}