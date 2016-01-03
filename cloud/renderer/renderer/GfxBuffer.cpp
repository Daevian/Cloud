#include "stdafx.h"
#include "GfxBuffer.h"

#include "RenderCore.h"

Cloud::Renderer::GfxBuffer::GfxBuffer()
    : GfxResource()
{
}

Cloud::Renderer::GfxBuffer::~GfxBuffer()
{
}

Cloud::Renderer::GfxBuffer::UniquePtr Cloud::Renderer::GfxBufferFactory::Create(const GfxBufferDesc& desc)
{
    if (!VerifySetup(desc))
    {
        CL_ASSERT_MSG("Failed to create the GfxBuffer!");
        return nullptr;
    }

    auto buffer = GfxBuffer::MakeUnique();

    buffer->m_desc = desc;

    D3D11_BUFFER_DESC bufDesc;
    ClMemZero(&bufDesc, sizeof(bufDesc));
    bufDesc.ByteWidth = desc.elementSize * desc.elementCount;
    bufDesc.BindFlags = desc.bindFlags;
    bufDesc.CPUAccessFlags = desc.cpuAccessFlags;
    bufDesc.MiscFlags = desc.miscFlags;
    bufDesc.Usage = desc.usage;
    bufDesc.StructureByteStride = (desc.miscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED) ? desc.elementSize : 0;

    bufDesc.ByteWidth = (desc.bindFlags & D3D10_BIND_CONSTANT_BUFFER) ? Math::RoundNearest<16>(bufDesc.ByteWidth) : bufDesc.ByteWidth;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = desc.initialData;

    ID3D11Buffer* buf;
    auto hr = GfxCore::Instance().GetDevice()->CreateBuffer( &bufDesc, desc.initialData ? &initData : nullptr, &buf);
    if (FAILED(hr))
    {
        CL_ASSERT_MSG("Failed to create the GfxBuffer!");
        buffer = nullptr;
        return nullptr;
    }

    buffer->m_buffer = Dx::MakeUnique(buf);

    RenderCore::SetDebugObjectName(buffer->m_buffer.get(), (desc.name + ".buf").c_str());

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

void Cloud::Renderer::GfxBufferFactory::InitSrv(const GfxBufferDesc& desc, GfxBuffer& buffer)
{
    CL_ASSERT_NULL(buffer.m_buffer);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ClMemZero(&srvDesc, sizeof(srvDesc));
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.BufferEx.FirstElement = 0;
    srvDesc.BufferEx.NumElements = desc.elementCount;

    ID3D11ShaderResourceView* srv;
    auto hr = GfxCore::Instance().GetDevice()->CreateShaderResourceView(buffer.m_buffer.get(), &srvDesc, &srv);
    if (FAILED(hr))
    {
        CL_ASSERT_MSG("Failed to create the GfxBuffer SRV!");
    }

    buffer.m_srv = Dx::MakeUnique(srv);

    RenderCore::SetDebugObjectName(buffer.m_srv.get(), (desc.name + ".srv").c_str());
}

void Cloud::Renderer::GfxBufferFactory::InitUav(const GfxBufferDesc& desc, GfxBuffer& buffer)
{
    CL_ASSERT_NULL(buffer.m_buffer);

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    ClMemZero(&uavDesc, sizeof(uavDesc));
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = desc.elementCount;

    ID3D11UnorderedAccessView* uav;
    auto hr = GfxCore::Instance().GetDevice()->CreateUnorderedAccessView(buffer.m_buffer.get(), &uavDesc, &uav);
    if (FAILED(hr))
    {
        CL_ASSERT_MSG("Failed to create the GfxBuffer UAV!");
    }

    buffer.m_uav = Dx::MakeUnique(uav);

    RenderCore::SetDebugObjectName(buffer.m_uav.get(), (desc.name + ".uav").c_str());
}

CLbool Cloud::Renderer::GfxBufferFactory::VerifySetup(const GfxBufferDesc& desc)
{
    CL_UNUSED(desc);
#ifdef _DEBUG
    if (desc.usage == D3D11_USAGE_DYNAMIC)
    {
        if (desc.cpuAccessFlags != D3D11_CPU_ACCESS_WRITE)
        {
            CL_ASSERT_MSG("Dynamic resources have to have CPU write access.");
            return false;
        }
    }
#endif

    return true;
}
