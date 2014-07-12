#include "stdafx.h"
#include "GfxBuffer.h"

#include "RenderCore.h"

Cloud::Renderer::GfxBuffer::GfxBuffer()
{
}



CLbool Cloud::Renderer::GfxBuffer::Init(void* initialData)
{
    D3D11_SUBRESOURCE_DATA* initialSRData = 0;
    if (initialData)
    {
        D3D11_SUBRESOURCE_DATA data;
        ClMemZero(&data, sizeof(data));
        data.pSysMem = initialData;

        initialSRData = &data;
    }

    D3D11_BUFFER_DESC desc;
    ClMemZero(&desc, sizeof(desc));
    
    desc.StructureByteStride = m_elementSize;
    desc.ByteWidth = m_elementSize * m_elementCount;
    //desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
    desc.Usage = D3D11_USAGE_DEFAULT;

    auto d3dDevice = RenderCore::Instance().GetDevice();
    auto result = d3dDevice->CreateBuffer(&desc, initialSRData, &m_buffer);

    if (FAILED(result))
    {
        CL_ASSERT_MSG("Couldn't create Gfx buffer!");
        return false;
    }

    return true;
}

void Cloud::Renderer::GfxBuffer::Uninit()
{
    if (m_buffer)
    {
        m_buffer->Release();
        m_buffer = 0;
    }
}