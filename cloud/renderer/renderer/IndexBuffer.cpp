#include "stdafx.h"
#include "IndexBuffer.h"

#include "RenderCore.h"

Cloud::Renderer::IndexBuffer::IndexBuffer(CLint indexSize)
    : m_indexCount(0)
    , m_indexSize(indexSize)
    , m_indexData(0)
{
}

Cloud::Renderer::IndexBuffer::IndexBuffer(IndexBuffer&& indexBuffer)
    : m_indexCount(indexBuffer.m_indexCount)
    , m_indexSize(indexBuffer.m_indexSize)
    , m_indexData(indexBuffer.m_indexData)
#ifdef USE_DIRECTX12
#else
    , m_indexBuffer(std::move(indexBuffer.m_indexBuffer))
#endif
{
    CL_ASSERT_MSG("move constructor needs updating!");
}

CLbool Cloud::Renderer::IndexBuffer::Initialise()
{
#ifdef USE_DIRECTX12
    // Note: using upload heaps to transfer static data like vert buffers is not 
    // recommended. Every time the GPU needs it, the upload heap will be marshalled 
    // over. Please read up on Default Heap usage. An upload heap is used here for 
    // code simplicity and because there are very few verts to actually transfer.
    auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto buffer = CD3DX12_RESOURCE_DESC::Buffer(m_indexSize * m_indexCount);

    if (FAILED(RenderCore::Instance().GetDevice()->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &buffer,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_buffer))))
    {
        CL_ASSERT_MSG("Couldn't create index buffer!");
        return false;
    }

    // copy initial index data
    {
        CD3DX12_RANGE readRange(0, 0); // no CPU read
        if (FAILED(m_buffer->Map(0, &readRange, &m_bufferData)))
        {
            CL_ASSERT_MSG("Couldn't map index buffer!");
            return false;
        }

        if (m_indexData)
        {
            memcpy(m_bufferData, m_indexData, m_indexSize * m_indexCount);
        }
    }

    m_view.BufferLocation = m_buffer->GetGPUVirtualAddress();
    m_view.Format = m_indexSize == sizeof(CLuint32) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
    m_view.SizeInBytes = m_indexSize * m_indexCount;

    return true;
#else
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = m_indexSize * m_indexCount;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    ClMemZero(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
    initData.pSysMem = m_indexData;

    ID3D11Buffer* dxBuf;
    HRESULT result = RenderCore::Instance().GetDevice()->CreateBuffer(&bufferDesc, &initData, &dxBuf);

    if (FAILED(result))
    {
        CL_ASSERT_MSG("Couldn't create vertex buffer!");
        return false;
    }

    m_indexBuffer = Dx::MakeUnique(dxBuf);

    return true;
#endif
}

void Cloud::Renderer::IndexBuffer::Uninitialise()
{
#ifdef USE_DIRECTX12
    if (m_buffer)
    {
        m_buffer->Unmap(0, nullptr);
    }
#else
    CL_ASSERT(m_indexBuffer != 0, "Can't unload uninitialised effect!");

    m_indexBuffer = nullptr;
#endif
}

void Cloud::Renderer::IndexBuffer::GPUUpdateIndexBuffer()
{
#ifdef USE_DIRECTX12
    memcpy(m_bufferData, m_indexData, m_indexSize * m_indexCount);
#else
    RenderCore::Instance().GetContext()->UpdateSubresource(
        m_indexBuffer.get(),
        0,
        nullptr,
        m_indexData,
        0,
        0);
#endif
}

void Cloud::Renderer::IndexBuffer::GPUUpdateIndexBuffer(void* data, CLsize_t size, CLsize_t offset)
{
    CL_ASSERT(offset + size <= m_indexCount * m_indexSize, "trying to copy outside of the buffer!");

    auto&& dest = static_cast<CLbyte*>(m_bufferData) + offset;
    memcpy(dest, data, size);
}
