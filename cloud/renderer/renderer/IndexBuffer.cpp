#include "stdafx.h"
#include "IndexBuffer.h"

#include "RenderCore.h"

Cloud::Renderer::IndexBuffer::IndexBuffer()
    : m_indexCount(0)
    , m_indexSize(sizeof(CLuint32))
    , m_indexData(0)
    , m_indexBuffer(0)
{
}

Cloud::Renderer::IndexBuffer::IndexBuffer(const IndexBuffer& vertexBuffer)
    : m_indexCount(vertexBuffer.m_indexCount)
    , m_indexSize(vertexBuffer.m_indexSize)
    , m_indexData(vertexBuffer.m_indexData)
    , m_indexBuffer(vertexBuffer.m_indexBuffer)
{
}

CLbool Cloud::Renderer::IndexBuffer::Initialise()
{
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = m_indexSize * m_indexCount;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    ClMemZero(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
    initData.pSysMem = m_indexData;

    HRESULT result = RenderCore::Instance().GetDevice()->CreateBuffer(&bufferDesc, &initData, &m_indexBuffer);

    if (FAILED(result))
    {
        CL_ASSERT_MSG("Couldn't create vertex buffer!");
        return false;
    }

    return true;
}

void Cloud::Renderer::IndexBuffer::Uninitialise()
{
    CL_ASSERT(m_indexBuffer != 0, "Can't unload uninitialised effect!");

    if (m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = 0;
    }
}

void Cloud::Renderer::IndexBuffer::GPUUpdateIndexBuffer()
{
    RenderCore::Instance().GetContext()->UpdateSubresource(
        m_indexBuffer,
        0,
        0,
        m_indexData,
        0,
        0);
}