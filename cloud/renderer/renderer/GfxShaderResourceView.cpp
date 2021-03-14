#include "stdafx.h"
#include "GfxShaderResourceView.h"

#include "RenderCore.h"
#include "GfxBuffer.h"
/*
Cloud::Renderer::GfxShaderResourceView::GfxShaderResourceView()
    : m_srv(0)
{
}

bool Cloud::Renderer::GfxShaderResourceView::Init(const GfxBuffer& buffer)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC desc;
    ClMemZero(&desc, sizeof(desc));

    desc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
    desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    desc.Buffer.FirstElement = 0;
    desc.Buffer.NumElements = buffer.GetElementCount();
    desc.Texture2D.MipLevels = 0;
    desc.Texture2D.MostDetailedMip = desc.Texture2D.MipLevels - 1;

    auto d3dDevice = RenderCore::Instance().GetDevice();
    auto result = d3dDevice->CreateShaderResourceView(buffer.GetBuffer(), &desc, &m_srv);

    if (FAILED(result))
    {
        CL_ASSERT_MSG("Couldn't create Shader Resource View!");
        return false;
    }

    return true;
}

void Cloud::Renderer::GfxShaderResourceView::Uninit()
{
    if (m_srv)
    {
        m_srv->Release();
        m_srv = 0;
    }
}*/