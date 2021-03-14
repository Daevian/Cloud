#include "stdafx.h"
#include "GfxInstanceBuffer.h"

#include "RenderCore.h"

Cloud::Renderer::GfxInstanceBuffer::GfxInstanceBuffer()
    : m_instanceCount(0)
    , m_instanceSize(0)
    , m_instanceData(0)
#ifdef USE_DIRECTX12
#else
    , m_instanceBuffer(0)
#endif
{
}

bool Cloud::Renderer::GfxInstanceBuffer::Initialise()
{
#ifdef USE_DIRECTX12
    return false;
#else
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
#endif
}

void Cloud::Renderer::GfxInstanceBuffer::Uninitialise()
{
#ifdef USE_DIRECTX12
#else
    CL_ASSERT(m_instanceBuffer != 0, "Can't unload uninitialised effect!");

    if (m_instanceBuffer)
    {
        m_instanceBuffer->Release();
        m_instanceBuffer = 0;
    }
#endif
}

void Cloud::Renderer::GfxInstanceBuffer::GpuSetInstanceBuffer()
{
#ifdef USE_DIRECTX12
#else
    uint stride = m_instanceSize;
    uint offset = 0;
    RenderCore::Instance().GetContext()->IASetVertexBuffers(0, 1, &m_instanceBuffer, &stride, &offset);
#endif
}

void Cloud::Renderer::GfxInstanceBuffer::GpuUpdateInstanceBuffer()
{
#ifdef USE_DIRECTX12
#else
    RenderCore::Instance().GetContext()->UpdateSubresource(
        m_instanceBuffer,
        0,
        0,
        m_instanceData,
        0,
        0);
#endif
}