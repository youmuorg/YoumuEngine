#include "d3d12_utils.h"

#include <mutex>

namespace base {
namespace win {
namespace dx {

void D3d12EnableDebugLayer() {
  static std::once_flag loadFlag;
  static PFN_D3D12_GET_DEBUG_INTERFACE d3d12GetDebugInterface = nullptr;

  std::call_once(loadFlag, []() -> void {
    HMODULE lib = ::LoadLibraryW(L"d3d12.dll");
    _ApiThrowIfNot("LoadLibraryW", lib != NULL);

    d3d12GetDebugInterface = 
        reinterpret_cast<PFN_D3D12_GET_DEBUG_INTERFACE>(
            ::GetProcAddress(lib, "D3D12GetDebugInterface"));
  });
  _ThrowIfNot(d3d12GetDebugInterface == nullptr);

  ComPtr<ID3D12Debug> debugController;
  HRESULT hr = d3d12GetDebugInterface(IID_PPV_ARGS(&debugController));
  _ComThrowIfError(hr);

  debugController->EnableDebugLayer();
}

D3d12Device::D3d12Device(IDXGIFactory4* dxgiFactory4) {
  // 查找高性能显卡
  ComPtr<IDXGIAdapter1> dxgiAdapter1;
  ComPtr<IDXGIFactory6> dxgiFactory6;
  HRESULT hr = dxgiFactory4->QueryInterface(IID_PPV_ARGS(&dxgiFactory6));
  _ComThrowIfError(hr);

  hr = dxgiFactory6->EnumAdapterByGpuPreference(
      0,
      DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
      IID_PPV_ARGS(&dxgiAdapter1));

  // 创建 D3D12 设备
  static std::once_flag loadFlag;
  static PFN_D3D12_CREATE_DEVICE d3d12CreateDevice = nullptr;

  std::call_once(loadFlag, []() -> void {
    HMODULE lib = ::LoadLibraryW(L"d3d12.dll");
    _ApiThrowIfNot("LoadLibraryW", lib != NULL);

    d3d12CreateDevice = 
        reinterpret_cast<PFN_D3D12_CREATE_DEVICE>(
            ::GetProcAddress(lib, "D3D12CreateDevice"));
  });
  _ThrowIfNot(d3d12CreateDevice == nullptr);

  hr = d3d12CreateDevice(
      dxgiAdapter1.Get(),
      D3D_FEATURE_LEVEL_11_0, 
      IID_PPV_ARGS(&_device));
  _ComThrowIfError(hr);

  // 创建同步围栏
  hr = _device->CreateFence(
      0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
  _ComThrowIfError(hr);

  // 检查 4X MSAA 抗锯齿支持
  D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msql;
  msql.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  msql.SampleCount = 4;
  msql.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
  msql.NumQualityLevels = 0;
  hr = _device->CheckFeatureSupport(
      D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
      &msql,
      sizeof(msql));
  _ComThrowIfError(hr);

  _ThrowIfNot(msql.NumQualityLevels > 0);

  // 创建命令队列
  D3D12_COMMAND_QUEUE_DESC queueDesc = {};
  queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
  queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

  hr = _device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_commandQueue));
  _ComThrowIfError(hr);

  hr = _device->CreateCommandAllocator(
      D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocator));
  _ComThrowIfError(hr);

  hr = _device->CreateCommandList(
      0,
      D3D12_COMMAND_LIST_TYPE_DIRECT,
      _commandAllocator.Get(),
      nullptr,
      IID_PPV_ARGS(&_commandList));
  _ComThrowIfError(hr);

}

D3d12Pipeline::D3d12Pipeline(
    ID3D12Device* device, 
    IDXGISwapChain1* swapChain1, 
    const D3d12PipelineConfig& config) {
  HRESULT hr = S_OK;

  // 获取视图描述符大小
  _rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(
      D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
  _dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(
      D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
  _cbvDescriptorSize = device->GetDescriptorHandleIncrementSize(
      D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

  // 创建视图描述符堆
  D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
  rtvHeapDesc.NumDescriptors = config.frameCount;
  rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  rtvHeapDesc.NodeMask = 0;
  hr = device->CreateDescriptorHeap(
      &rtvHeapDesc, IID_PPV_ARGS(&_rtvHeap));
  _ComThrowIfError(hr);

  D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
  dsvHeapDesc.NumDescriptors = 1;
  dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
  dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  dsvHeapDesc.NodeMask = 0;
  hr = device->CreateDescriptorHeap(
      &dsvHeapDesc, IID_PPV_ARGS(&_dsvHeap));
  _ComThrowIfError(hr);
}

}
}
}