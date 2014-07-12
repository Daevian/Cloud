#include "stdafx.h"
#include "GfxConstantBuffer.h"

#include "RenderCore.h"

Cloud::Renderer::GfxConstantBuffer::GfxConstantBuffer()
    : m_data(0)
    , m_dataSize(0)
    , m_constantBuffer(0)
{
}

CLbool Cloud::Renderer::GfxConstantBuffer::Initialise()
{
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
}

void Cloud::Renderer::GfxConstantBuffer::Uninitialise()
{
    if (m_constantBuffer)
    {
        m_constantBuffer->Release();
        m_constantBuffer = 0;
    }
}

void Cloud::Renderer::GfxConstantBuffer::SetData(void* data, CLint dataSize)
{
    m_data = data;
    m_dataSize = dataSize;
}

void Cloud::Renderer::GfxConstantBuffer::GPUUpdateConstantBuffer()
{
    RenderCore::Instance().GetContext()->UpdateSubresource(m_constantBuffer, 0, 0, m_data, 0, 0 );
}