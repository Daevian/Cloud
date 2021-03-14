#include "stdafx.h"
#include "ParticleManager.h"

#include "RenderCore.h"
#include "ShaderEffect.h"
#include "Texture.h"

Cloud::Renderer::ParticleManager::ParticleManager()
{
}

Cloud::Renderer::ParticleManager::~ParticleManager()
{
}

bool Cloud::Renderer::ParticleManager::Initialise()
{
    m_effect = RenderCore::Instance().GetEffectContainer().GetEffect("data/effects/pfx.effect");
    m_texture = RenderCore::Instance().GetTextureContainer().GetTexture("data/textures/main_menu_cursor.dds");

    m_vertexBuffer.SetVertexCount(m_particles.Count());
    m_vertexBuffer.SetVertexSize(sizeof(ParticleVertex));
    m_vertexBuffer.SetVertexData((t_char*)&m_particleVertexData);
//    m_vertexBuffer.SetTopology(GfxPrimitiveTopology::Pointlist);

    if (!m_vertexBuffer.Initialise()) return false;

    for (uint i = 0; i < m_particles.Count(); i++)
    {
        float velX = (ClRandFloat() - 0.5f) * 0.05f;
        float velY = (ClRandFloat() - 0.5f) * 0.05f;
        float velZ = (ClRandFloat() - 0.5f) * 0.05f;

        m_particles[i].position = float4(0.0f, 0.0f, 0.0f, 1.0f);
        m_particles[i].velocity = float4(velX, velY, velZ, 0.0f);

        m_gpuParticles[i].position = float4(0.0f, 0.0f, 0.0f, 1.0f);
        m_gpuParticles[i].velocity = float4(velX, velY, velZ, 0.0f);
    }

#ifdef USE_DIRECTX12
    return false;
#else
    // GPU sim input buffer
    auto d3dDevice = RenderCore::Instance().GetDevice();
    HRESULT result;
    {
        D3D11_SUBRESOURCE_DATA initialSRData;
        ClMemZero(&initialSRData, sizeof(initialSRData));
        initialSRData.pSysMem = &m_gpuParticles[0];

        D3D11_BUFFER_DESC desc;
        ClMemZero(&desc, sizeof(desc));
    
        desc.StructureByteStride = sizeof(GpuParticle);
        desc.ByteWidth = m_gpuParticles.SizeOf();
        desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        desc.Usage = D3D11_USAGE_DEFAULT;

        ID3D11Buffer* dxBuf;
        result = d3dDevice->CreateBuffer(&desc, &initialSRData, &dxBuf);

        m_buffer = Dx::MakeUnique(dxBuf);
    }

    // GPU fill input buffer
    {
        D3D11_BUFFER_DESC desc;
        ClMemZero(&desc, sizeof(desc));

        desc.StructureByteStride = sizeof(GpuParticle);
        desc.ByteWidth = m_gpuParticles.SizeOf();
        desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; //possible?
        desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        desc.Usage = D3D11_USAGE_DEFAULT;

        ID3D11Buffer* dxBuf;
        result = d3dDevice->CreateBuffer(&desc, 0, &dxBuf);
        m_bufferOut = Dx::MakeUnique(dxBuf);
    }

    // GPU sim output buffer
    {
        D3D11_BUFFER_DESC desc;
        ClMemZero(&desc, sizeof(desc));

        desc.StructureByteStride = sizeof(ParticleVertex);
        desc.ByteWidth = m_gpuParticles.Count() * sizeof(ParticleVertex);
        desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        desc.Usage = D3D11_USAGE_DEFAULT;

        ID3D11Buffer* dxBuf;
        result = d3dDevice->CreateBuffer(&desc, 0, &dxBuf);
        m_bufferVertexBuffer = Dx::MakeUnique(dxBuf);
    }

    // GPU sim input view
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC desc;
        ClMemZero(&desc, sizeof(desc));

        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        desc.Buffer.FirstElement = 0;
        desc.Buffer.NumElements = m_gpuParticles.Count();

        ID3D11ShaderResourceView* dxSrv;
        result = d3dDevice->CreateShaderResourceView(m_buffer.get(), &desc, &dxSrv);
        m_bufferSRV = Dx::MakeUnique(dxSrv);
    }

    // GPU sim output view
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
        ClMemZero(&desc, sizeof(desc));

        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        desc.Buffer.FirstElement = 0;
        desc.Buffer.NumElements = m_gpuParticlesOut.Count();

        ID3D11UnorderedAccessView* dxUav;
        result = d3dDevice->CreateUnorderedAccessView(m_bufferOut.get(), &desc, &dxUav);
        m_bufferOutUAV = Dx::MakeUnique(dxUav);
    }

    // GPU fill input
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
        ClMemZero(&desc, sizeof(desc));

        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        desc.Buffer.FirstElement = 0;
        desc.Buffer.NumElements = m_gpuParticlesOut.Count();

        ID3D11UnorderedAccessView* dxUav;
        result = d3dDevice->CreateUnorderedAccessView(m_bufferOut.get(), &desc, &dxUav);
        m_fillView = Dx::MakeUnique(dxUav);
    }

    // GPU fill output
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
        ClMemZero(&desc, sizeof(desc));

        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        desc.Buffer.FirstElement = 0;
        desc.Buffer.NumElements = m_vertexBuffer.GetVertexCount();

        ID3D11UnorderedAccessView* dxUav;
        result = d3dDevice->CreateUnorderedAccessView(m_bufferVertexBuffer.get(), &desc, &dxUav);
        m_fillOutView = Dx::MakeUnique(dxUav);
    }

    // GPU vertex buffer view
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC desc;
        ClMemZero(&desc, sizeof(desc));

        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        desc.Buffer.FirstElement = 0;
        desc.Buffer.NumElements = m_gpuParticles.Count();

        ID3D11ShaderResourceView* dxSrv;
        result = d3dDevice->CreateShaderResourceView(m_bufferVertexBuffer.get(), &desc, &dxSrv);
        m_vertexBufferView = Dx::MakeUnique(dxSrv);
    }

    // Compile particle sim shader

    {
        //D3D10_SHADER_MACRO
        ID3D10Blob* blob = 0;
        ID3D10Blob* errorBlob = 0;

        result = D3DCompileFromFile(
            L"data/shaders/compute/ParticleSim.hlsl",
            nullptr,
            nullptr,
            "main",
            "cs_5_0",
            0,
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
                t_char* error = static_cast<t_char*>(errorBlob->GetBufferPointer());
                CL_TRACE_CHANNEL("ERROR", error);
            }

            CL_ASSERT_MSG(assertMessage.str().c_str());

            return false;
        }

        ID3D11ComputeShader* dxShader;
        result = d3dDevice->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), 0, &dxShader);
        m_simShader = Dx::MakeUnique(dxShader);
    }

     // Compile particle sim shader

    {
        //D3D10_SHADER_MACRO
        ID3D10Blob* blob = 0;
        ID3D10Blob* errorBlob = 0;

        result = D3DCompileFromFile(
            L"data/shaders/compute/ParticleFill.hlsl",
            nullptr,
            nullptr,
            "main",
            "cs_5_0",
            0,
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
                t_char* error = static_cast<t_char*>(errorBlob->GetBufferPointer());
                CL_TRACE_CHANNEL("ERROR", error);
            }

            CL_ASSERT_MSG(assertMessage.str().c_str());

            return false;
        }

        ID3D11ComputeShader* dxShader;
        result = d3dDevice->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), 0, &dxShader);
        m_fillShader = Dx::MakeUnique(dxShader);
    }


    return true;
#endif
}

void Cloud::Renderer::ParticleManager::Uninitialise()
{
    m_vertexBuffer.Uninitialise();
}

void Cloud::Renderer::ParticleManager::Update(float timeStep)
{
    float4 timeStepVector(timeStep);

    for (uint particleIndex = 0, count = m_particles.Count(); particleIndex < count; ++particleIndex)
    {
        Particle& particle = m_particles[particleIndex];
        particle.position += particle.velocity * timeStepVector;
    }

#ifdef USE_DIRECTX12
#else
    // GPU update
    auto context = RenderCore::Instance().GetContext();

    context->CSSetShader(m_simShader.get(), 0, 0);

    ID3D11ShaderResourceView* srViews[1] = { m_bufferSRV.get() };
    context->CSSetShaderResources(0, 1, srViews);

    ID3D11UnorderedAccessView* uaViews[1] = { m_bufferOutUAV.get() };
    context->CSSetUnorderedAccessViews(0, 1, uaViews, 0);

    context->Dispatch(256, 1, 1);        


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

void Cloud::Renderer::ParticleManager::Fill()
{
    for (uint i = 0; i < m_particles.Count(); i++)
    {
        Particle& particle = m_particles[i];
        ParticleVertex& vertex = m_particleVertexData[i];

        vertex.pos.x = particle.position.x;
        vertex.pos.y = particle.position.y;
        vertex.pos.z = particle.position.z;
        vertex.scale.x = 0.1f;
        vertex.scale.y = 0.1f;
    }

    m_vertexBuffer.GPUUpdateVertexBuffer();

#ifdef USE_DIRECTX12
#else
    // GPU update
    auto context = RenderCore::Instance().GetContext();

    context->CSSetShader(m_fillShader.get(), 0, 0);

    ID3D11UnorderedAccessView* uaViews[2] = { m_fillView.get(), m_fillOutView.get() };
    context->CSSetUnorderedAccessViews(0, 2, uaViews, 0);

    context->Dispatch(256, 1, 1);
#endif

}

void Cloud::Renderer::ParticleManager::Render()
{
#ifdef USE_DIRECTX12
#else
    RenderingDevice& renderingDevice = RenderCore::Instance().GetRenderingDevice();
    auto context = RenderCore::Instance().GetContext();


  /*  renderingDevice.SetTexture(m_texture);
    renderingDevice.SetSamplerState(m_texture ? m_texture->GetSamplerState() : nullptr, 0);
    renderingDevice.SetEffect(m_effect);
    renderingDevice.SetVertexBuffer(&m_vertexBuffer);

    renderingDevice.Render();
*/


    // gpu particle render

    renderingDevice.SetTexturePS(m_texture ? m_texture->GetTexture() : nullptr);
    renderingDevice.SetSamplerState(m_texture ? m_texture->GetSamplerState() : nullptr, 0);
    renderingDevice.SetEffect(m_effect);
    renderingDevice.SetVertexBuffer(&m_vertexBuffer);

    auto* vbSrv = m_vertexBufferView.get();
    context->VSSetShaderResources(1, 1, &vbSrv);
    renderingDevice.Draw();
    context->VSSetShaderResources(1, 1, 0);
#endif
}