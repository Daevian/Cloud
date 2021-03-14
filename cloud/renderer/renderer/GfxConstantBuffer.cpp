#include "stdafx.h"
#include "GfxConstantBuffer.h"

#include "RenderCore.h"
#include "utilities/MathUtilities.h"

Cloud::Renderer::GfxConstantBuffer::GfxConstantBuffer()
{
}

bool Cloud::Renderer::GfxConstantBuffer::Initialise(uint versions)
{
    m_versions = versions;

#ifdef USE_DIRECTX12

    auto&& device = RenderCore::Instance().GetDevice();

    const auto alignedDataSize = CL_ALIGN_TO(m_dataSize, c_cbufferAlignment); // CB size is required to be 256-byte aligned.

    auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto buffer = CD3DX12_RESOURCE_DESC::Buffer(alignedDataSize * m_versions);

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

    /*static const auto c_cbvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    const auto offset = RenderCore::Instance().m_cbvHeapIndex++;
    m_handle.InitOffsetted(RenderCore::Instance().GetCbvHeap()->GetCPUDescriptorHandleForHeapStart(), offset, c_cbvDescriptorSize);
    

    D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
    desc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
    desc.SizeInBytes = alignedDataSize;
    device->CreateConstantBufferView(&desc, m_handle);*/

    // We don't unmap this until the app closes. Keeping things mapped for the lifetime of the resource is okay.
    CD3DX12_RANGE readRange(0, 0);  // No CPU read
    if (FAILED(m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_bufferData))))
    {
        CL_ASSERT_MSG("Couldn't map constant buffer!");
        return false;
    }

    if (m_data)
    {
        UpdateConstantBuffer(m_data);
    }

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

bool Cloud::Renderer::GfxConstantBuffer::Initialise(int dataSize, const void* initialData, uint versions)
{
    SetData(initialData, dataSize);
    return Initialise(versions);
}

void Cloud::Renderer::GfxConstantBuffer::Uninitialise()
{
    m_versions = 0;
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

void Cloud::Renderer::GfxConstantBuffer::SetData(const void* data, int dataSize)
{
    m_data = data;
    m_dataSize = dataSize;
}

void Cloud::Renderer::GfxConstantBuffer::GPUUpdateConstantBuffer()
{
#ifdef USE_DIRECTX12
    if (m_data)
    {
        UpdateConstantBuffer(m_data);
    }
#else
    RenderCore::Instance().GetContext()->UpdateSubresource(m_constantBuffer, 0, 0, m_data, 0, 0 );
#endif
}

D3D12_GPU_VIRTUAL_ADDRESS Cloud::Renderer::GfxConstantBuffer::UpdateConstantBuffer(const void* data)
{
    m_currentVersion++;
    CL_ASSERT(m_currentVersion < static_cast<int>(m_versions), "ran out of versions!");

    const auto alignedDataSize = CL_ALIGN_TO(m_dataSize, c_cbufferAlignment);
    const auto offset = m_currentVersion * alignedDataSize;
    ClMemCopy(m_bufferData + offset, data, m_dataSize);

    return m_constantBuffer->GetGPUVirtualAddress() + offset;
}

D3D12_GPU_VIRTUAL_ADDRESS Cloud::Renderer::GfxConstantBuffer::GetCurrentVersionGpuAddress() const
{
    const auto alignedDataSize = CL_ALIGN_TO(m_dataSize, c_cbufferAlignment);

    // even if there are no versions, at least get a valid address
    const auto offset = std::max(m_currentVersion, 0) * alignedDataSize;
    auto temp = m_constantBuffer->GetGPUVirtualAddress() + offset;
    return temp;
}

void Cloud::Renderer::GfxConstantBuffer::Reset()
{
    m_currentVersion = -1;
}
