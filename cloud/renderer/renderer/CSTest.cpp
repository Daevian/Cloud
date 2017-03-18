#include "stdafx.h"
#include "CSTest.h"

#include "RenderCore.h"
#include "ShaderEffect.h"
#include "Texture.h"

Cloud::Renderer::CSTest::CSTest()
{
}

Cloud::Renderer::CSTest::~CSTest()
{
}

CLbool Cloud::Renderer::CSTest::Initialise()
{
#ifdef USE_DIRECTX12
#else
    if (!InitVBIB())
        return false;

    // TEXTURE
    {
        std::array<CLuint32, 16> pixels = { 0 };
        for (CLuint i = 0; i < 16; i += 8)
        {
            pixels[i + 0] = 0xFFFF0000;
            pixels[i + 1] = 0xFF00FF00;
            pixels[i + 2] = 0xFF0000FF;
            pixels[i + 3] = 0xFFFF00FF;
            pixels[i + 4] = 0xFFFF00FF;
            pixels[i + 5] = 0xFF0000FF;
            pixels[i + 6] = 0xFF00FF00;
            pixels[i + 7] = 0xFFFF0000;
        }

        D3D11_SUBRESOURCE_DATA data;
        ClMemZero(&data, sizeof(data));
        data.SysMemPitch = 4 * sizeof(CLuint32);
        data.pSysMem = &pixels[0];

        D3D11_TEXTURE2D_DESC desc;
        ClMemZero(&desc, sizeof(desc));
        desc.Width = 4;
        desc.Height = 4;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

        auto device = RenderCore::Instance().GetDevice();
        ID3D11Texture2D* dxTex;
        auto result = device->CreateTexture2D(&desc, &data, &dxTex);

        if (FAILED(result))
        {
            CL_ASSERT_MSG("Couldn't create Texture2D Resource!");
            return false;
        }

        m_textureResource = Dx::MakeUnique(dxTex);
    }

    // SRV
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC desc;
        ClMemZero(&desc, sizeof(desc));
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipLevels = 1;
        desc.Texture2D.MostDetailedMip = 0;

        auto device = RenderCore::Instance().GetDevice();
        ID3D11ShaderResourceView* dxSrv;
        auto result = device->CreateShaderResourceView(m_textureResource.get(), &desc, &dxSrv);

        if (FAILED(result))
        {
            CL_ASSERT_MSG("Couldn't create Texture2D SRV!");
            return false;
        }

        m_textureSRV = Dx::MakeUnique(dxSrv);
    }

    // UAV
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
        ClMemZero(&desc, sizeof(desc));

        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipSlice = 0;

        auto device = RenderCore::Instance().GetDevice();
        ID3D11UnorderedAccessView* dxUav;
        auto result = device->CreateUnorderedAccessView(m_textureResource.get(), &desc, &dxUav);

        if (FAILED(result))
        {
            CL_ASSERT_MSG("Couldn't create Texture2D UAV!");
            return false;
        }

        m_textureUAV = Dx::MakeUnique(dxUav);
    }

    // SAMPLER
    {
        D3D11_SAMPLER_DESC samplerDesc;
        ClMemZero(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        ID3D11SamplerState* dxSampler;
        auto result = RenderCore::Instance().GetDevice()->CreateSamplerState( &samplerDesc, &dxSampler);
        
        if (FAILED(result))
        {
            CL_ASSERT_MSG("Couldn't create Sampler State!");
            return false;
        }

        m_samplerState = Dx::MakeUnique(dxSampler);
    }

    // SHADER
    {
        //D3D10_SHADER_MACRO
        ID3D10Blob* blob = 0;
        ID3D10Blob* errorBlob = 0;

        CLdword shaderFlags = 0;//D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
        shaderFlags |= D3DCOMPILE_DEBUG;
        shaderFlags |= D3DCOMPILE_PREFER_FLOW_CONTROL;
        shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        HRESULT result = D3DCompileFromFile(
            L"data/shaders/compute/cstest.hlsl",
            nullptr,
            nullptr,
            "main",
            "cs_5_0",
            shaderFlags,
            0,
            &blob,
            &errorBlob);

        if (FAILED(result) || errorBlob)
        {
            std::stringstream assertMessage;
            //assertMessage << "Couldn't load " << shaderPath;

            //CL_TRACE_CHANNEL("ERROR", assertMessage.str().c_str());

            if (errorBlob)
            {
                CLchar* error = static_cast<CLchar*>(errorBlob->GetBufferPointer());
                CL_TRACE_CHANNEL("ERROR", error);
            }

            CL_ASSERT_MSG(assertMessage.str().c_str());

            return false;
        }

        auto device = RenderCore::Instance().GetDevice();
        ID3D11ComputeShader* dxShader;
        result = device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), 0, &dxShader);
        m_csShader = Dx::MakeUnique(dxShader);
    }
#endif
    return true;
}

CLbool Cloud::Renderer::CSTest::InitVBIB()
{
    m_effect = RenderCore::Instance().GetEffectContainer().GetEffect("data/effects/cstest.effect");
    //m_texture = RenderCore::Instance().GetTextureContainer().GetTexture("data/textures/main_menu_cursor.dds");

    m_vertexData[0].pos.Set(-1.0f, 1.0f, 0.0f, 1.0f);
    m_vertexData[0].uv.Set(0.0f, 0.0f);
    m_vertexData[1].pos.Set(1.0f, 1.0f, 0.0f, 1.0f);
    m_vertexData[1].uv.Set(1.0f, 0.0f);
    m_vertexData[2].pos.Set(-1.0f, -1.0f, 0.0f, 1.0f);
    m_vertexData[2].uv.Set(0.0f, 1.0f);
    m_vertexData[3].pos.Set(1.0f, -1.0f, 0.0f, 1.0f);
    m_vertexData[3].uv.Set(1.0f, 1.0f);

    m_indexData[0] = 0;
    m_indexData[1] = 1;
    m_indexData[2] = 2;
    m_indexData[3] = 2;
    m_indexData[4] = 1;
    m_indexData[5] = 3;

    m_vertexBuffer.SetVertexCount(m_vertexData.Count());
    m_vertexBuffer.SetVertexSize(sizeof(Vertex));
    m_vertexBuffer.SetVertexData((CLchar*)&m_vertexData);
    //m_vertexBuffer.SetTopology(GfxPrimitiveTopology::Trianglelist);
    if (!m_vertexBuffer.Initialise())
        return false;

    m_indexBuffer.SetIndexCount(m_indexData.Count());
    m_indexBuffer.SetIndexData((CLchar*)&m_indexData);
    if (!m_indexBuffer.Initialise())
        return false;

    return true;
}

void Cloud::Renderer::CSTest::Uninitialise()
{
    m_indexBuffer.Uninitialise();
    m_vertexBuffer.Uninitialise();

#ifdef USE_DIRECTX12
#else
    m_textureResource = nullptr;
    m_textureSRV = nullptr;
    m_textureUAV = nullptr;
    m_samplerState = nullptr;
    m_csShader = nullptr;
#endif
}

void Cloud::Renderer::CSTest::Update()
{
#ifdef USE_DIRECTX12
#else
    auto context = RenderCore::Instance().GetContext();

    context->CSSetShader(m_csShader.get(), 0, 0);

    //ID3D11ShaderResourceView* srViews[1] = { m_textureSRV };
    //context->CSSetShaderResources(0, 1, srViews);

    ID3D11UnorderedAccessView* uaViews[1] = { m_textureUAV.get() };
    context->CSSetUnorderedAccessViews(0, 1, uaViews, 0);

    context->Dispatch(4, 4, 1);

    //ID3D11ShaderResourceView* nullsrv[1] = { 0 };
    //context->CSSetShaderResources(0, 1, nullsrv);

    ID3D11UnorderedAccessView* nulluav[1] = { 0 };
    context->CSSetUnorderedAccessViews(0, 1, nulluav, 0);


   /* ID3D11Buffer* buffer = 0;
    D3D11_BUFFER_DESC desc;
    ClMemZero(&desc, sizeof(desc));
    m_bufferOut->GetDesc(&desc);

    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;

    auto result = RenderCore::Instance().GetDevice()->CreateBuffer(&desc, 0, &buffer);
    context->CopyResource(buffer, m_bufferOut);

    D3D11_MAPPED_SUBRESOURCE resource;
    result = context->Map(buffer, 0, D3D11_MAP_READ, 0, &resource);

    ClMemCopy(m_gpuParticlesOut.GetBuffer(), resource.pData, m_gpuParticlesOut.SizeOf());

    context->Unmap(buffer, 0);*/
#endif
}

void Cloud::Renderer::CSTest::Render()
{
#ifdef USE_DIRECTX12
#else
    RenderingDevice& renderingDevice = RenderCore::Instance().GetRenderingDevice();

   // renderingDevice.SetTexture(m_textureSRV, 0);
    renderingDevice.SetSamplerState(m_samplerState.get(), 0);
    //renderingDevice.SetTexture(m_texture);
    renderingDevice.SetEffect(m_effect);
    renderingDevice.SetVertexBuffer(&m_vertexBuffer);
    renderingDevice.SetIndexBuffer(&m_indexBuffer);

    renderingDevice.DrawIndexed();
#endif
}