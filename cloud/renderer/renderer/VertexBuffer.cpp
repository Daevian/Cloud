#include "stdafx.h"
#include "VertexBuffer.h"

#include "RenderCore.h"

Cloud::Renderer::VertexBuffer::VertexBuffer()
:m_vertexCount(0)
,m_vertexSize(0)
,m_vertexData(0)
,m_topology(D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED)
,m_vertexBuffer(0)
{
}

Cloud::Renderer::VertexBuffer::VertexBuffer(const VertexBuffer& vertexBuffer)
:m_vertexCount(vertexBuffer.m_vertexCount)
,m_vertexSize(vertexBuffer.m_vertexSize)
,m_vertexData(vertexBuffer.m_vertexData)
,m_topology(vertexBuffer.m_topology)
,m_vertexBuffer(vertexBuffer.m_vertexBuffer)
{
}

CLbool Cloud::Renderer::VertexBuffer::Init()
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

    HRESULT result = RenderCore::Instance().GetDevice()->CreateBuffer(&bufferDesc, &initData, &m_vertexBuffer);

    if (FAILED(result))
    {
        CL_ASSERT_MSG("Couldn't create vertex buffer!");
        return false;
    }

    return true;
}

void Cloud::Renderer::VertexBuffer::Unload()
{
    CL_ASSERT(m_vertexBuffer != 0, "Can't unload uninitialised effect!");

    if (m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = 0;
    }
}

void Cloud::Renderer::VertexBuffer::GPUSetVertexBuffer()
{
    CLuint stride = m_vertexSize;
    CLuint offset = 0;
    RenderCore::Instance().GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
}

void Cloud::Renderer::VertexBuffer::GPUSetTopology()
{
    RenderCore::Instance().GetDeviceContext()->IASetPrimitiveTopology(m_topology);
}

void Cloud::Renderer::VertexBuffer::GPUUpdateVertexBuffer()
{
    RenderCore::Instance().GetDeviceContext()->UpdateSubresource(   m_vertexBuffer,
                                                                    0,
                                                                    0,
                                                                    m_vertexData,
                                                                    0,
                                                                    0);
}