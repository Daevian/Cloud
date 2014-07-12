#include "stdafx.h"
#include "GfxInstanceBuffer.h"

#include "RenderCore.h"

Cloud::Renderer::GfxInstanceBuffer::GfxInstanceBuffer()
    : m_instanceCount(0)
    , m_instanceSize(0)
    , m_instanceData(0)
    , m_instanceBuffer(0)
{
}

CLbool Cloud::Renderer::GfxInstanceBuffer::Initialise()
{
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = m_instanceSize * m_instanceCount;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    ClMemZero(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
    initData.pSysMem = m_instanceData;

    HRESULT result = RenderCore::Instance().GetDevice()->CreateBuffer(&bufferDesc, &initData, &m_instanceBuffer);

    if (FAILED(result))
    {
        CL_ASSERT_MSG("Couldn't create vertex buffer!");
        return false;
    }

    return true;
}

void Cloud::Renderer::GfxInstanceBuffer::Uninitialise()
{
    CL_ASSERT(m_instanceBuffer != 0, "Can't unload uninitialised effect!");

    if (m_instanceBuffer)
    {
        m_instanceBuffer->Release();
        m_instanceBuffer = 0;
    }
}

void Cloud::Renderer::GfxInstanceBuffer::GpuSetInstanceBuffer()
{
    CLuint stride = m_instanceSize;
    CLuint offset = 0;
    RenderCore::Instance().GetContext()->IASetVertexBuffers(0, 1, &m_instanceBuffer, &stride, &offset);
}

void Cloud::Renderer::GfxInstanceBuffer::GpuUpdateInstanceBuffer()
{
    RenderCore::Instance().GetContext()->UpdateSubresource(
        m_instanceBuffer,
        0,
        0,
        m_instanceData,
        0,
        0);
}