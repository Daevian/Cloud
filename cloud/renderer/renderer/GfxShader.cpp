#include "stdafx.h"
#include "GfxShader.h"
#include "RenderCore.h"

Cloud::Renderer::GfxComputeShader::GfxComputeShader()
    : GfxResource()
{
}

Cloud::Renderer::GfxComputeShader::~GfxComputeShader()
{
}

CLbool Cloud::Renderer::GfxShaderFactory::CompileShader(const ClString& /*shaderPath*/, const ClString& /*entryPoint*/, const ClString& /*shaderModel*/, GfxShaderBlob& /*shaderBlobOutput*/)
{
#ifdef USE_DIRECTX12
    return false;
#else
    shaderBlobOutput.blob = nullptr;

    CLdword shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3DCOMPILE_DEBUG;
    shaderFlags |= D3DCOMPILE_PREFER_FLOW_CONTROL;
    shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* errorBlob = 0;
    HRESULT result = D3DCompileFromFile(
        ClWString(shaderPath.begin(), shaderPath.end()).c_str(),
        nullptr,
        nullptr,
        entryPoint.c_str(),
        shaderModel.c_str(),
        shaderFlags,
        0,
        &shaderBlobOutput.blob,
        &errorBlob);

    if (FAILED(result) || errorBlob)
    {
        std::stringstream assertMessage;
        assertMessage << "Couldn't load " << shaderPath;

        CL_TRACE_CHANNEL("ERROR", assertMessage.str().c_str());

        if (errorBlob)
        {
            CLchar* error = static_cast<CLchar*>(errorBlob->GetBufferPointer());
            CL_TRACE_CHANNEL("ERROR", error);
            errorBlob->Release();
        }

        CL_ASSERT_MSG(assertMessage.str().c_str());
        

        return false;
    }

    return true;
#endif
}

Cloud::Renderer::GfxComputeShader::UniquePtr Cloud::Renderer::GfxShaderFactory::Create(const GfxComputerShaderDesc& /*desc*/)
{
#ifdef USE_DIRECTX12
    return nullptr;
#else
    auto shader = GfxComputeShader::MakeUnique();

    CL_ASSERT_NULL(desc.shaderBlob.blob);

    shader->m_desc = desc;

    ID3D11ComputeShader* dxShader;
    HRESULT result = GfxCore::Instance().GetDevice()->CreateComputeShader(desc.shaderBlob.blob->GetBufferPointer(), desc.shaderBlob.blob->GetBufferSize(), 0, &dxShader);
    if (FAILED(result))
    {
        CL_ASSERT_MSG("Failed to create compute shader!");
        shader = nullptr;
    }

    shader->m_shader = Dx::MakeUnique(dxShader);
    
    RenderCore::SetDebugObjectName(shader->m_shader.get(), (desc.name + ".cs").c_str());

    return shader;
#endif
}
