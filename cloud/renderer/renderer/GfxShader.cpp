#include "stdafx.h"
#include "GfxShader.h"
#include "RenderCore.h"

Cloud::Renderer::GfxComputeShader::GfxComputeShader()
    : m_shader(nullptr)
{
}

CLbool Cloud::Renderer::GfxShaderFactory::CompileShader(const ClString& shaderPath, const ClString& entryPoint, const ClString& shaderModel, GfxShaderBlob& shaderBlobOutput)
{
    shaderBlobOutput.blob = nullptr;

    CLdword shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3DCOMPILE_DEBUG;
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
}

Cloud::Renderer::GfxComputeShader* Cloud::Renderer::GfxShaderFactory::Create(const GfxComputerShaderDesc& desc)
{
    GfxComputeShader* shader = new GfxComputeShader();

    CL_ASSERT_NULL(desc.shaderBlob.blob);

    shader->m_desc = desc;

    HRESULT result = GfxCore::Instance().GetDevice()->CreateComputeShader(desc.shaderBlob.blob->GetBufferPointer(), desc.shaderBlob.blob->GetBufferSize(), 0, &shader->m_shader);
    if (FAILED(result))
    {
        CL_ASSERT_MSG("Failed to create compute shader!");
        SAFE_DELETE(shader);
    }

    RenderCore::SetDebugObjectName(shader->m_shader, (desc.name + ".cs").c_str());

    return shader;
}

void Cloud::Renderer::GfxShaderFactory::Destroy(GfxComputeShader* shader)
{
    if (shader)
    {
        shader->m_shader->Release();
    }

    delete shader;
}
