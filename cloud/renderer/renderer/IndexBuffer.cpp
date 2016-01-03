#include "stdafx.h"
#include "IndexBuffer.h"

#include "RenderCore.h"

Cloud::Renderer::IndexBuffer::IndexBuffer()
    : m_indexCount(0)
    , m_indexSize(sizeof(CLuint32))
    , m_indexData(0)
{
}

Cloud::Renderer::IndexBuffer::IndexBuffer(IndexBuffer&& vertexBuffer)
    : m_indexCount(vertexBuffer.m_indexCount)
    , m_indexSize(vertexBuffer.m_indexSize)
    , m_indexData(vertexBuffer.m_indexData)
    , m_indexBuffer(std::move(vertexBuffer.m_indexBuffer))
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

    ID3D11Buffer* dxBuf;
    HRESULT result = RenderCore::Instance().GetDevice()->CreateBuffer(&bufferDesc, &initData, &dxBuf);

    if (FAILED(result))
    {
        CL_ASSERT_MSG("Couldn't create vertex buffer!");
        return false;
    }

    m_indexBuffer = Dx::MakeUnique(dxBuf);

    return true;
}

void Cloud::Renderer::IndexBuffer::Uninitialise()
{
    CL_ASSERT(m_indexBuffer != 0, "Can't unload uninitialised effect!");

    m_indexBuffer = nullptr;
}

void Cloud::Renderer::IndexBuffer::GPUUpdateIndexBuffer()
{
    RenderCore::Instance().GetContext()->UpdateSubresource(
        m_indexBuffer.get(),
        0,
        nullptr,
        m_indexData,
        0,
        0);
}