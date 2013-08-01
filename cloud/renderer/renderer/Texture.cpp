#include "stdafx.h"
#include "Texture.h"
#include "RenderCore.h"

Cloud::Renderer::Texture::Texture(const std::string& texturePath)
: m_textureRV(0)
, m_samplerState(0)
, m_texturePath(texturePath)
{
}

CLbool Cloud::Renderer::Texture::Load()
{
    if (!LoadResource()) return false;
    if (!LoadSampler()) return false;

    return true;
}

void Cloud::Renderer::Texture::Unload()
{
    CL_ASSERT(m_textureRV != 0, "Can't unload uninitialised texture!");
    CL_ASSERT(m_samplerState != 0, "Can't unload uninitialised texture!");

    if (m_textureRV)
    {
        m_textureRV->Release();
        m_textureRV = 0;
    }

    if (m_samplerState)
    {
        m_samplerState->Release();
        m_samplerState = 0;
    }
}

void Cloud::Renderer::Texture::GPUSetShaderResource()
{
    RenderCore::Instance().GetDeviceContext()->PSSetShaderResources(0, 1, &m_textureRV);
}

void Cloud::Renderer::Texture::GPUSetSamplerState()
{
    RenderCore::Instance().GetDeviceContext()->PSSetSamplers( 0, 1, &m_samplerState);
}

CLbool Cloud::Renderer::Texture::LoadResource()
{
    CL_ASSERT(m_textureRV == 0, (m_texturePath + " already loaded!").c_str());

    HRESULT result;
    result = D3DX11CreateShaderResourceViewFromFile(RenderCore::Instance().GetDevice(),
                                                    m_texturePath.c_str(),
                                                    0,
                                                    0,
                                                    &m_textureRV,
                                                    0);

    if (FAILED(result))
    {
        std::stringstream assertMessage;
        assertMessage << "Couldn't load " << m_texturePath;

        CL_TRACE_CHANNEL("ERROR", assertMessage.str().c_str());
        CL_ASSERT_MSG(assertMessage.str().c_str());
        return false;
    }

    return true;
}

CLbool Cloud::Renderer::Texture::LoadSampler()
{
    D3D11_SAMPLER_DESC samplerDesc;
    ClMemZero(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    auto result = RenderCore::Instance().GetDevice()->CreateSamplerState( &samplerDesc, &m_samplerState);
    if(FAILED(result))
    {
        std::stringstream assertMessage;
        assertMessage << "Couldn't create sampler for " << m_texturePath;

        CL_TRACE_CHANNEL("ERROR", assertMessage.str().c_str());
        CL_ASSERT_MSG(assertMessage.str().c_str());
        return false;
    }

    return true;
}