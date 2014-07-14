#include "stdafx.h"
#include "GfxBuffer.h"

#include "RenderCore.h"

Cloud::Renderer::GfxStructuredBuffer::GfxStructuredBuffer()
    : m_buffer(nullptr)
    , m_srv(nullptr)
    , m_uav(nullptr)
{
}

Cloud::Renderer::GfxStructuredBuffer* Cloud::Renderer::GfxBufferFactory::Create(const GfxStructuredBufferDesc& desc)
{
    GfxStructuredBuffer* buffer = new GfxStructuredBuffer();
    CL_ASSERT_NULL(buffer);

    buffer->m_desc = desc;

    buffer->m_buffer = nullptr;

    D3D11_BUFFER_DESC bufDesc;
    ClMemZero(&bufDesc, sizeof(bufDesc));
    bufDesc.BindFlags = desc.bindFlags;
    bufDesc.ByteWidth = desc.elementSize * desc.elementCount;
    bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bufDesc.StructureByteStride = desc.elementSize;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = desc.initialData;
    auto hr = GfxCore::Instance().GetDevice()->CreateBuffer( &bufDesc, desc.initialData ? &initData : nullptr, &buffer->m_buffer);
    if (FAILED(hr))
    {
        CL_ASSERT_MSG("Failed to create the Structured Buffer!");
        Destroy(buffer);
        return nullptr;
    }

    RenderCore::SetDebugObjectName(buffer->m_buffer, (desc.name + ".sbuf").c_str());

    if (desc.bindFlags & D3D11_BIND_SHADER_RESOURCE)
    {
        InitSrv(desc, *buffer);
    }

    if (desc.bindFlags & D3D11_BIND_UNORDERED_ACCESS)
    {
        InitUav(desc, *buffer);
    }

    return buffer;
}

void Cloud::Renderer::GfxBufferFactory::Destroy(GfxStructuredBuffer* buffer)
{
    if (buffer->m_buffer)
    {
        buffer->m_buffer->Release();
    }

    if (buffer->m_srv)
    {
        buffer->m_srv->Release();
    }

    if (buffer->m_uav)
    {
        buffer->m_uav->Release();
    }
    
    delete buffer;
}

void Cloud::Renderer::GfxBufferFactory::InitSrv(const GfxStructuredBufferDesc& desc, GfxStructuredBuffer& buffer)
{
    CL_ASSERT_NULL(buffer.m_buffer);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ClMemZero(&srvDesc, sizeof(srvDesc));
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.BufferEx.FirstElement = 0;
    srvDesc.BufferEx.NumElements = desc.elementCount;

    auto hr = GfxCore::Instance().GetDevice()->CreateShaderResourceView(buffer.m_buffer, &srvDesc, &buffer.m_srv);
    if (FAILED(hr))
    {
        CL_ASSERT_MSG("Failed to create the Structured Buffer SRV!");
    }

    RenderCore::SetDebugObjectName(buffer.m_srv, (desc.name + ".srv").c_str());
}

void Cloud::Renderer::GfxBufferFactory::InitUav(const GfxStructuredBufferDesc& desc, GfxStructuredBuffer& buffer)
{
    CL_ASSERT_NULL(buffer.m_buffer);

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    ClMemZero(&uavDesc, sizeof(uavDesc));
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = desc.elementCount;

    auto hr = GfxCore::Instance().GetDevice()->CreateUnorderedAccessView(buffer.m_buffer, &uavDesc, &buffer.m_uav);
    if (FAILED(hr))
    {
        CL_ASSERT_MSG("Failed to create the Structured Buffer UAV!");
    }

    RenderCore::SetDebugObjectName(buffer.m_uav, (desc.name + ".uav").c_str());
}
