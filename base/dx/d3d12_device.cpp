#include "d3d12_device.h"

#include <mutex>

namespace base {
namespace dx {

HMODULE _GetD3d12Lib() {
  static std::once_flag loadFlag;
  static HMODULE lib = nullptr;

  std::call_once(loadFlag, []() { lib = ::LoadLibraryW(L"d3d12.dll"); });
  _ThrowIfError("LoadLibraryW", lib != nullptr);
  return lib;
}

PFN_D3D12_SERIALIZE_ROOT_SIGNATURE _GetD3d12SerializeRootSignatureFun() {
  PFN_D3D12_SERIALIZE_ROOT_SIGNATURE d3d12SerializeRootSignature =
      reinterpret_cast<PFN_D3D12_SERIALIZE_ROOT_SIGNATURE>(
          ::GetProcAddress(_GetD3d12Lib(), "D3D12SerializeRootSignature"));
  _ThrowIfError("GetProcAddress", d3d12SerializeRootSignature != nullptr);
  return d3d12SerializeRootSignature;
}

void D3d12Device::EnableDebugLayer() {
  PFN_D3D12_GET_DEBUG_INTERFACE d3d12GetDebugInterface =
      reinterpret_cast<PFN_D3D12_GET_DEBUG_INTERFACE>(
          ::GetProcAddress(_GetD3d12Lib(), "D3D12GetDebugInterface"));
  _ThrowIfError("GetProcAddress", d3d12GetDebugInterface != nullptr);

  ComPtr<ID3D12Debug> debugController;
  HRESULT hr = d3d12GetDebugInterface(IID_PPV_ARGS(&debugController));
  _ThrowIfFailed(hr);

  debugController->EnableDebugLayer();
}

void D3d12Device::CreateDevice(IDXGIFactory1* dxgiFactory) {
#if defined(_DEBUG)
  EnableDebugLayer();
#endif

  PFN_D3D12_CREATE_DEVICE d3d12CreateDevice =
      reinterpret_cast<PFN_D3D12_CREATE_DEVICE>(
          ::GetProcAddress(_GetD3d12Lib(), "D3D12CreateDevice"));
  _ThrowIfError("GetProcAddress", d3d12CreateDevice != nullptr);

  ComPtr<IDXGIAdapter1> adapter;
  ComPtr<IDXGIFactory6> dxgiFactory6;

  if (SUCCEEDED(dxgiFactory->QueryInterface(IID_PPV_ARGS(&dxgiFactory6)))) {
    // 查找高性能显卡
    for (uint32_t index = 0;; ++index) {
      HRESULT hr = dxgiFactory6->EnumAdapterByGpuPreference(
          index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
      if (FAILED(hr)) {
        break;
      }

      DXGI_ADAPTER_DESC1 desc;
      adapter->GetDesc1(&desc);
      if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
        // 不要软渲染
        continue;
      }

      // 检查是否支持 DX12，但不要创建
      hr = d3d12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
                             _uuidof(ID3D12Device), nullptr);
      if (SUCCEEDED(hr)) {
        break;
      }
    }
  } else {
    for (uint32_t index = 0;; ++index) {
      HRESULT hr = dxgiFactory->EnumAdapters1(index, &adapter);
      if (FAILED(hr)) {
        break;
      }

      DXGI_ADAPTER_DESC1 desc;
      adapter->GetDesc1(&desc);
      if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
        // 不要软渲染
        continue;
      }

      // 检查是否支持 DX12，但不要创建
      hr = d3d12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
                             _uuidof(ID3D12Device), nullptr);
      if (SUCCEEDED(hr)) {
        break;
      }
    }
  }
  _ThrowMessageIfNot(adapter.Get() != nullptr, "No available GPU found.");

  // 创建 D3D12 设备
  HRESULT hr = d3d12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
                                 IID_PPV_ARGS(&_device));
  _ThrowIfFailed(hr);

  // 检查 4X MSAA 抗锯齿支持
  D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msql;
  msql.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  msql.SampleCount = 4;
  msql.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
  msql.NumQualityLevels = 0;
  hr = _device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
                                    &msql, sizeof(msql));
  _ThrowIfFailed(hr);

  _ThrowMessageIfNot(msql.NumQualityLevels > 0,
                     "Unexpected MSAA quality level.");

  // 创建命令队列
  D3D12_COMMAND_QUEUE_DESC queueDesc = {};
  queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
  queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

  hr = _device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_commandQueue));
  _ThrowIfFailed(hr);
}

void D3d12Device::CreateSwapchainForHwnd(IDXGIFactory2* dxgiFactory,
                                         HWND hwnd) {
  _ThrowIfNull(_commandQueue.Get());

  // 双缓冲
  _frameCount = 2;

  DXGI_SWAP_CHAIN_DESC1 desc = {0};
  // 自适应窗口大小
  desc.Width = 0;
  desc.Height = 0;
  desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  desc.Stereo = FALSE;
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;
  desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT |
                     DXGI_USAGE_BACK_BUFFER;
  desc.BufferCount = _frameCount;
  desc.Scaling = DXGI_SCALING_STRETCH;
  desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
  desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
  desc.Flags = 0;
  desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

  ComPtr<IDXGISwapChain1> swapchain1;
  HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(
      _commandQueue.Get(), hwnd, &desc, nullptr, nullptr,
      swapchain1.ReleaseAndGetAddressOf());
  _ThrowIfFailed(hr);

  // 禁用全屏快捷键
  hr = dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
  _ThrowIfFailed(hr);

  hr = swapchain1.As(&_swapchain);
  _ThrowIfFailed(hr);

  _frameIndex = _swapchain->GetCurrentBackBufferIndex();

  // 获取缓冲区 size
  ComPtr<ID3D12Resource> buffer;
  hr = _swapchain->GetBuffer(_frameIndex, IID_PPV_ARGS(&buffer));
  _ThrowIfFailed(hr);
  D3D12_RESOURCE_DESC bufferDesc = buffer->GetDesc();
  // _viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(bufferDesc.Width), static_cast<float>(bufferDesc.Height));
  // _scissorRect = CD3DX12_RECT(0, 0, static_cast<LONG>(bufferDesc.Width), static_cast<LONG>(bufferDesc.Height));
  _viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(bufferDesc.Width), static_cast<float>(bufferDesc.Height));
  _scissorRect = CD3DX12_RECT(0, 0, static_cast<LONG>(bufferDesc.Width), static_cast<LONG>(bufferDesc.Height));
}

void D3d12Device::CreateFence() {
  // 创建同步围栏
  HRESULT hr =
      _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
  _ThrowIfFailed(hr);

  _fenceValue = 1;
  _fenceEvent.Create();
}

void D3d12Device::CreateCommandList() {
  HRESULT hr = _device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                       IID_PPV_ARGS(&_commandAllocator));
  _ThrowIfFailed(hr);

  // 创建命令列表
  hr = _device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                  _commandAllocator.Get(), nullptr,
                                  IID_PPV_ARGS(&_commandList));
  _ThrowIfFailed(hr);

  // 命令列表创建时默认处于记录状态，先关闭。
  hr = _commandList->Close();
  _ThrowIfFailed(hr);
}

// 创建视图描述符堆
void D3d12Device::CreateDescriptorHeaps() {
  D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
  rtvHeapDesc.NumDescriptors = _frameCount;
  rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  rtvHeapDesc.NodeMask = 0;
  HRESULT hr = _device->CreateDescriptorHeap(
      &rtvHeapDesc, IID_PPV_ARGS(&_rtvHeap));
  _ThrowIfFailed(hr);

  D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
  dsvHeapDesc.NumDescriptors = _frameCount;
  dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
  dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  dsvHeapDesc.NodeMask = 0;
  hr = _device->CreateDescriptorHeap(
      &dsvHeapDesc, IID_PPV_ARGS(&_dsvHeap));
  _ThrowIfFailed(hr);

  // 获取描述符大小
  _rtvDescriptorSize = _device->GetDescriptorHandleIncrementSize(
      D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
  _dsvDescriptorSize = _device->GetDescriptorHandleIncrementSize(
      D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
  _cbvDescriptorSize = _device->GetDescriptorHandleIncrementSize(
      D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void D3d12Device::CreateRtv() {
  CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
      _rtvHeap->GetCPUDescriptorHandleForHeapStart());

  _rtvBuffers.resize(_frameCount);
  for (UINT n = 0; n < _frameCount; n++) {
    HRESULT hr = _swapchain->GetBuffer(n, IID_PPV_ARGS(&_rtvBuffers[n]));
    _ThrowIfFailed(hr);
    _device->CreateRenderTargetView(_rtvBuffers[n].Get(), nullptr, rtvHandle);
    rtvHandle.Offset(1, _rtvDescriptorSize);
  }
}

void D3d12Device::CreateDsv() {
  D3D12_RESOURCE_DESC depthStencilDesc;
  depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

  CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
      _rtvHeap->GetCPUDescriptorHandleForHeapStart());

  _rtvBuffers.resize(_frameCount);
  for (UINT n = 0; n < _frameCount; n++) {
    HRESULT hr = _swapchain->GetBuffer(n, IID_PPV_ARGS(&_rtvBuffers[n]));
    _ThrowIfFailed(hr);
    _device->CreateRenderTargetView(_rtvBuffers[n].Get(), nullptr, rtvHandle);
    rtvHandle.Offset(1, _rtvDescriptorSize);
  }
}

void D3d12Device::CreateRootSignature() {
  // 创建根签名
  CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
  rootSignatureDesc.Init(
      0, nullptr, 0, nullptr,
      D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

  ComPtr<ID3DBlob> signature;
  ComPtr<ID3DBlob> error;
  HRESULT hr = _GetD3d12SerializeRootSignatureFun()(
      &rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
  hr = _device->CreateRootSignature(0, signature->GetBufferPointer(),
                                   signature->GetBufferSize(),
                                   IID_PPV_ARGS(&_rootSignature));
  _ThrowIfFailed(hr);

}

void D3d12Device::CreatePipeline() {
  CreateCommandList();
  CreateDescriptorHeaps();
  CreateFence();
  CreateRtv();
  CreateRootSignature();
}

void D3d12Device::BeginPopulateCommandList() {
  // 必须在 GPU 执行完成关联的命令列表，才能重置此分配器。
  HRESULT hr = _commandAllocator->Reset();
  _ThrowIfFailed(hr);

  // 重置命令列表
  hr = _commandList->Reset(_commandAllocator.Get(), nullptr);
  _ThrowIfFailed(hr);

  _commandList->SetGraphicsRootSignature(_rootSignature.Get());
  _commandList->RSSetViewports(1, &_viewport);
  _commandList->RSSetScissorRects(1, &_scissorRect);

  // 将资源从呈现状态转换为渲染目标状态
  CD3DX12_RESOURCE_BARRIER resourceBarrierRenderTarget =
      CD3DX12_RESOURCE_BARRIER::Transition(_rtvBuffers[_frameIndex].Get(),
                                           D3D12_RESOURCE_STATE_PRESENT,
                                           D3D12_RESOURCE_STATE_RENDER_TARGET);
  _commandList->ResourceBarrier(1, &resourceBarrierRenderTarget);

  CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
      _rtvHeap->GetCPUDescriptorHandleForHeapStart(), _frameIndex,
      _rtvDescriptorSize);
  _commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
  // 清屏
  const float clearColor[] = {0.0f, 0.4f, 0.5f, 1.0f};
  _commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}

void D3d12Device::EndPopulateCommandList() {
  // 再次转换资源
  CD3DX12_RESOURCE_BARRIER resourceBarrierPresent =
      CD3DX12_RESOURCE_BARRIER::Transition(_rtvBuffers[_frameIndex].Get(),
                                           D3D12_RESOURCE_STATE_RENDER_TARGET,
                                           D3D12_RESOURCE_STATE_PRESENT);
  _commandList->ResourceBarrier(1, &resourceBarrierPresent);

  // 命令记录完成
  HRESULT hr = _commandList->Close();
  _ThrowIfFailed(hr);
}

// 记录命令
void D3d12Device::PopulateCommandList() {
}

void D3d12Device::ExecuteCommandList() {
  ID3D12CommandList* commandLists[] = { _commandList.Get() };
  _commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
}

void D3d12Device::Present() {
  HRESULT hr = _swapchain->Present(1, 0);
  _ThrowIfFailed(hr);
}

void D3d12Device::WaitCommandList() {
  const uint64_t fence = _fenceValue;
  HRESULT hr = _commandQueue->Signal(_fence.Get(), fence);
  _ThrowIfFailed(hr);

  _fenceValue++;

  // 等待 GPU 完成
  if (_fence->GetCompletedValue() < fence) {
    hr = _fence->SetEventOnCompletion(fence, _fenceEvent.handle());
    ::WaitForSingleObject(_fenceEvent.handle(), INFINITE);
  }

  _frameIndex = _swapchain->GetCurrentBackBufferIndex();
}

}  // namespace dx
}  // namespace base