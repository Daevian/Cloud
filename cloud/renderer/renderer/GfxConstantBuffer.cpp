#include "stdafx.h"
#include "GfxConstantBuffer.h"

#include "RenderCore.h"

Cloud::Renderer::GfxConstantBuffer::GfxConstantBuffer()
    : m_data(0)
    , m_dataSize(0)
#ifdef USE_DIRECTX12
#else
    , m_constantBuffer(0)
#endif
{
}

CLbool Cloud::Renderer::GfxConstantBuffer::Initialise()
{
#ifdef USE_DIRECTX12

    auto&& device = RenderCore::Instance().GetDevice();

    const auto alignedDataSize = CL_ALIGN_TO(m_dataSize, 256); // CB size is required to be 256-byte aligned.

    auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto buffer = CD3DX12_RESOURCE_DESC::Buffer(alignedDataSize);

    if (FAILED(device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &buffer,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_constantBuffer))))
    {
        CL_ASSERT_MSG("Couldn't create constant buffer!");
        return false;
    }

    static auto s_descriptorIndex = 0;
    static const auto c_cbvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    m_handle.InitOffsetted(RenderCore::Instance().GetCbvHeap()->GetCPUDescriptorHandleForHeapStart(), s_descriptorIndex++, c_cbvDescriptorSize);
    

    D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
    desc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
    desc.SizeInBytes = alignedDataSize;
    device->CreateConstantBufferView(&desc, m_handle);

    // We don't unmap this until the app closes. Keeping things mapped for the lifetime of the resource is okay.
    CD3DX12_RANGE readRange(0, 0);  // No CPU read
    if (FAILED(m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_bufferData))))
    {
        CL_ASSERT_MSG("Couldn't map constant buffer!");
        return false;
    }

    ClMemCopy(m_bufferData, m_data, m_dataSize);

    return true;
#else
    D3D11_BUFFER_DESC constantBufferDesc;
    ClMemZero(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    constantBufferDesc.ByteWidth = m_dataSize;
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = 0;
    auto result = RenderCore::Instance().GetDevice()->CreateBuffer(&constantBufferDesc, 0, &m_constantBuffer);
    if( FAILED( result) )
    {
        CL_ASSERT_MSG("Couldn't create constant buffer!");
        return false;
    }

    return true;
#endif
}

void Cloud::Renderer::GfxConstantBuffer::Uninitialise()
{
#ifdef USE_DIRECTX12
    if (m_constantBuffer)
    {
        m_constantBuffer->Unmap(0, nullptr);
    }
#else
    if (m_constantBuffer)
    {
        m_constantBuffer->Release();
        m_constantBuffer = 0;
    }
#endif
}

void Cloud::Renderer::GfxConstantBuffer::SetData(void* data, CLint dataSize)
{
    m_data = data;
    m_dataSize = dataSize;
}

void Cloud::Renderer::GfxConstantBuffer::GPUUpdateConstantBuffer()
{
#ifdef USE_DIRECTX12
    ClMemCopy(m_bufferData, m_data, m_dataSize);
#else
    RenderCore::Instance().GetContext()->UpdateSubresource(m_constantBuffer, 0, 0, m_data, 0, 0 );
#endif
}