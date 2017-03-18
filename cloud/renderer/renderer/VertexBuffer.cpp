#include "stdafx.h"
#include "VertexBuffer.h"

#include "RenderCore.h"

Cloud::Renderer::VertexBuffer::VertexBuffer()
    : m_vertexCount(0)
    , m_vertexSize(0)
    , m_vertexData(0)
//    , m_topology(GfxPrimitiveTopology::Undefined)
{
}

Cloud::Renderer::VertexBuffer::VertexBuffer(VertexBuffer&& vertexBuffer)
    : m_vertexCount(vertexBuffer.m_vertexCount)
    , m_vertexSize(vertexBuffer.m_vertexSize)
    , m_vertexData(vertexBuffer.m_vertexData)
#ifdef USE_DIRECTX12
#else
    , m_topology(vertexBuffer.m_topology)
    , m_vertexBuffer(vertexBuffer.m_vertexBuffer.release())
#endif
{
    CL_ASSERT_MSG("move constructor needs updating!");
}

CLbool Cloud::Renderer::VertexBuffer::Initialise()
 {
#ifdef USE_DIRECTX12

    // Note: using upload heaps to transfer static data like vert buffers is not 
    // recommended. Every time the GPU needs it, the upload heap will be marshalled 
    // over. Please read up on Default Heap usage. An upload heap is used here for 
    // code simplicity and because there are very few verts to actually transfer.
    auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto buffer = CD3DX12_RESOURCE_DESC::Buffer(m_vertexSize * m_vertexCount);

    if (FAILED(RenderCore::Instance().GetDevice()->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &buffer,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_buffer))))
    {
        CL_ASSERT_MSG("Couldn't create vertex buffer!");
        return false;
    }

    // copy initial vertex data
    {
        CD3DX12_RANGE readRange(0, 0); // no CPU read
        if (FAILED(m_buffer->Map(0, &readRange, &m_bufferData)))
        {
            CL_ASSERT_MSG("Couldn't map vertex buffer!");
            return false;
        }

        memcpy(m_bufferData, m_vertexData, m_vertexSize * m_vertexCount);
    }

    m_view.BufferLocation = m_buffer->GetGPUVirtualAddress();
    m_view.StrideInBytes = m_vertexSize;
    m_view.SizeInBytes = m_vertexSize * m_vertexCount;

    return true;
#else
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = m_vertexSize * m_vertexCount;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    ClMemZero(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
    initData.pSysMem = m_vertexData;

    ID3D11Buffer* dxBuf;
    HRESULT result = RenderCore::Instance().GetDevice()->CreateBuffer(&bufferDesc, &initData, &dxBuf);

    if (FAILED(result))
    {
        CL_ASSERT_MSG("Couldn't create vertex buffer!");
        return false;
    }

    m_vertexBuffer = Dx::MakeUnique(dxBuf);

    return true;
#endif
}

void Cloud::Renderer::VertexBuffer::Uninitialise()
{
#ifdef USE_DIRECTX12
    if (m_buffer)
    {
        m_buffer->Unmap(0, nullptr);
    }
#else
    CL_ASSERT(m_vertexBuffer != 0, "Can't unload uninitialised effect!");

    m_vertexBuffer = nullptr;
#endif
}

void Cloud::Renderer::VertexBuffer::GPUUpdateVertexBuffer()
{
#ifdef USE_DIRECTX12
    memcpy(m_bufferData, m_vertexData, m_vertexSize * m_vertexCount);
#else
    RenderCore::Instance().GetContext()->UpdateSubresource(
        m_vertexBuffer.get(),
        0,
        nullptr,
        m_vertexData,
        0,
        0);
#endif
}