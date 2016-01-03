#include "stdafx.h"
#include "VertexBuffer.h"

#include "RenderCore.h"

Cloud::Renderer::VertexBuffer::VertexBuffer()
    : m_vertexCount(0)
    , m_vertexSize(0)
    , m_vertexData(0)
    , m_topology(GfxPrimitiveTopology::Undefined)
{
}

Cloud::Renderer::VertexBuffer::VertexBuffer(VertexBuffer&& vertexBuffer)
    : m_vertexCount(vertexBuffer.m_vertexCount)
    , m_vertexSize(vertexBuffer.m_vertexSize)
    , m_vertexData(vertexBuffer.m_vertexData)
    , m_topology(vertexBuffer.m_topology)
    , m_vertexBuffer(vertexBuffer.m_vertexBuffer.release())
{
}

CLbool Cloud::Renderer::VertexBuffer::Initialise()
{
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
}

void Cloud::Renderer::VertexBuffer::Uninitialise()
{
    CL_ASSERT(m_vertexBuffer != 0, "Can't unload uninitialised effect!");

    m_vertexBuffer = nullptr;
}

void Cloud::Renderer::VertexBuffer::GPUUpdateVertexBuffer()
{
    RenderCore::Instance().GetContext()->UpdateSubresource(
        m_vertexBuffer.get(),
        0,
        nullptr,
        m_vertexData,
        0,
        0);
}