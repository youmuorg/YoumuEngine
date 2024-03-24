#pragma once

#include "d3d12_device.h"
#include "dx_common.h"
#include "hlsl_compiler.h"

#include <DirectXMath.h>

#include <string>

namespace base::dx {

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;

std::wstring _GetAssetsPath();

// 简单色彩 IA
class D3d12SimpleColorInputLayout {
public:
  struct Vertex {
    XMFLOAT3 pos;
    XMFLOAT4 color;
  };

public:
  D3D12_INPUT_LAYOUT_DESC inputLayoutDesc() {
    return D3D12_INPUT_LAYOUT_DESC{.pInputElementDescs = _inputElementDescs,
                                   .NumElements = _countof(_inputElementDescs)};
  }

private:
  D3D12_INPUT_ELEMENT_DESC _inputElementDescs[2] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
       D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
      {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
       D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};
};

// 顶点着色
class D3d12Shader {
public:
  void LoadFromFile(const wchar_t* filePath,
                    const char* entryPoint,
                    const char* target) {
    std::wstring assetsPath = _GetAssetsPath();
    std::wstring assetsFilePath = assetsPath + filePath;
    HlslCompiler::CompileShader(assetsFilePath.c_str(), entryPoint, target,
                                &_bytecode);
  }

  D3D12_SHADER_BYTECODE shaderBytecode() {
    return CD3DX12_SHADER_BYTECODE(_bytecode.Get());
  }

private:
  ComPtr<ID3DBlob> _bytecode;
};

// 几何体网格
class D3d12MeshGeometry {
public:
  void CreateVertexBufferFromMemory(ID3D12Device* device,
                                    ID3D12GraphicsCommandList* commandList,
                                    void* buffer,
                                    size_t bufferSize,
                                    size_t bufferStride) {
    _bufferSize = bufferSize;
    _bufferStride = bufferStride;

    // 显存资源缓冲
    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties, D3D12_HEAP_FLAG_NONE, &desc,
        D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&_vertexBuffer));
    _ThrowIfFailed(hr);

    // 显存上传缓冲
    CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
    ComPtr<ID3D12Resource> vertexBufferUploadHeap;
    hr = device->CreateCommittedResource(
        &uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&vertexBufferUploadHeap));
    _ThrowIfFailed(hr);

    // 拷贝顶点数据到上传缓冲，再拷贝到通用缓冲
    D3D12_SUBRESOURCE_DATA vertexData = {};
    vertexData.pData = buffer;
    vertexData.RowPitch = bufferSize;
    vertexData.SlicePitch = bufferStride;

    UpdateSubresources<1>(commandList, _vertexBuffer.Get(),
                          vertexBufferUploadHeap.Get(), 0, 0, 1, &vertexData);

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    commandList->ResourceBarrier(1, &barrier);
  }

  void CreateVertexBufferFromFile(ID3D12Device* device,
                                  const wchar_t* filePath,
                                  size_t bufferSize,
                                  size_t bufferStride) {}

  void CreateIndexBufferFromMemory(ID3D12Device* device,
                                   ID3D12GraphicsCommandList* commandList,
                                   void* index,
                                   size_t indexSize,
                                   size_t indexCount,
                                   DXGI_FORMAT indexFormat) {
    _indexSize = indexSize;
    _indexFormat = indexFormat;
    _indexCount = indexCount;

    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(indexSize);
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties, D3D12_HEAP_FLAG_NONE, &desc,
        D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&_indexBuffer));
    _ThrowIfFailed(hr);

    CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
    ComPtr<ID3D12Resource> indexBufferUploadHeap;
    hr = device->CreateCommittedResource(
        &uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&indexBufferUploadHeap));
    _ThrowIfFailed(hr);

    // 拷贝顶点数据到上传缓冲，再拷贝到通用缓冲
    D3D12_SUBRESOURCE_DATA indexData = {};
    indexData.pData = index;
    indexData.RowPitch = indexSize;
    indexData.SlicePitch = indexData.RowPitch;

    UpdateSubresources<1>(commandList, _indexBuffer.Get(),
                          indexBufferUploadHeap.Get(), 0, 0, 1, &indexData);

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_INDEX_BUFFER);
    commandList->ResourceBarrier(1, &barrier);
  }

  void CreateTextureFromMemory(ID3D12Device* device,
                               ID3D12GraphicsCommandList* commandList,
                               uint16_t mipLevels,
                               DXGI_FORMAT format,
                               uint32_t width,
                               uint32_t height,
                               void* data,
                               size_t dataSize,
                               size_t dataStride) {
    _textureFormat = format;

    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = mipLevels;
    textureDesc.Format = format;
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties, D3D12_HEAP_FLAG_NONE, &textureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&_texture));
    _ThrowIfFailed(hr);

    const UINT subresourceCount =
        textureDesc.DepthOrArraySize * textureDesc.MipLevels;
    const UINT64 uploadBufferSize =
        GetRequiredIntermediateSize(_texture.Get(), 0, subresourceCount);

    ComPtr<ID3D12Resource> textureUploadHeap;
    CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC uploadDesc =
        CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
    hr = device->CreateCommittedResource(
        &uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &uploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&textureUploadHeap));

    D3D12_SUBRESOURCE_DATA textureData = {};
    textureData.pData = data;
    textureData.RowPitch = dataSize;
    textureData.SlicePitch = dataStride;

    UpdateSubresources(commandList, _texture.Get(), textureUploadHeap.Get(), 0,
                       0, subresourceCount, &textureData);

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &barrier);
  }

  void CreateSampler(ID3D12Device* device, ID3D12DescriptorHeap* samplerHeap) {
    D3D12_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    device->CreateSampler(&samplerDesc,
                          samplerHeap->GetCPUDescriptorHandleForHeapStart());
  }

  void CreateShaderResourceView(ID3D12Device* device,
                                ID3D12DescriptorHeap* cbvSrvHeap) {
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = _textureFormat;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    device->CreateShaderResourceView(
        _texture.Get(), &srvDesc,
        cbvSrvHeap->GetCPUDescriptorHandleForHeapStart());
  }

  void CreateDepthStencilView(ID3D12Device* device,
                              ID3D12DescriptorHeap* dsvHeap,
                              uint32_t width,
                              uint32_t height) {
    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
    depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

    D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
    depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
    depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
        DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties, D3D12_HEAP_FLAG_NONE, &desc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthOptimizedClearValue,
        IID_PPV_ARGS(&_depthStencil));
    _ThrowIfFailed(hr);

    device->CreateDepthStencilView(
        _depthStencil.Get(), &depthStencilDesc,
        dsvHeap->GetCPUDescriptorHandleForHeapStart());
  }

  D3D12_VERTEX_BUFFER_VIEW vertexBufferView() {
    return D3D12_VERTEX_BUFFER_VIEW{
        .BufferLocation = _vertexBuffer->GetGPUVirtualAddress(),
        .SizeInBytes = static_cast<UINT>(_bufferSize),
        .StrideInBytes = static_cast<UINT>(_bufferStride),
    };
  }

  D3D12_INDEX_BUFFER_VIEW indexBufferView() {
    return D3D12_INDEX_BUFFER_VIEW{
        .BufferLocation = _indexBuffer->GetGPUVirtualAddress(),
        .SizeInBytes = static_cast<UINT>(_indexSize),
        .Format = _indexFormat};
  }

  size_t indexCount() const { return _indexCount; }

private:
  ComPtr<ID3D12Resource> _vertexBuffer;
  size_t _bufferSize = 0;
  size_t _bufferStride = 0;

  ComPtr<ID3D12Resource> _indexBuffer;
  size_t _indexSize = 0;
  size_t _indexCount = 0;
  DXGI_FORMAT _indexFormat = DXGI_FORMAT_UNKNOWN;

  ComPtr<ID3D12Resource> _texture;
  DXGI_FORMAT _textureFormat;

  ComPtr<ID3D12Resource> _depthStencil;
};

// 几何体
class BoxGeometry {};

class D3d12Pipeline {};

}  // namespace base::dx