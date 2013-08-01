#include "stdafx.h"
#include "ShaderEffect.h"

#include "RenderCore.h"
#include "Texture.h"

Cloud::Renderer::ShaderEffect::ShaderEffect()
: m_vertexShader(0)
, m_geometryShader(0)
, m_pixelShader(0)
, m_blendState(0)
{
}

CLbool Cloud::Renderer::ShaderEffect::Load(const std::string& effectPath)
{
    CL_ASSERT(m_vertexShader == 0, ("vertex shader in " + effectPath + " already loaded!").c_str());
    CL_ASSERT(m_geometryShader == 0, ("geometry shader in " + effectPath + " already loaded!").c_str());
    CL_ASSERT(m_pixelShader == 0, ("pixel shader in " + effectPath + " already loaded!").c_str());

    Json::Value root;
    std::ifstream jsonFile(effectPath.c_str());
    Json::Reader reader;

    CLbool parseSuccessful = reader.parse(jsonFile, root);
    if (!parseSuccessful)
    {
        std::stringstream assertMessage;
        assertMessage << "Failed to parse " << effectPath << " file" << std::endl << reader.getFormatedErrorMessages();

        CL_TRACE_CHANNEL("ERROR", assertMessage.str().c_str());
        CL_ASSERT_MSG("Failed to parse effect file!");
        return false;
    }

    std::string shaderPath = root["Shader"].asString();
    if (shaderPath == "")
    {
        std::stringstream assertMessage;
        assertMessage << "Shader missing in '" << effectPath << "'";

        CL_TRACE_CHANNEL("ERROR", assertMessage.str().c_str());
        CL_ASSERT_MSG(assertMessage.str().c_str());
        return false;
    }

    InputLayout::InputLayoutDesc inputLayoutDesc;
    ParseInputLayout(root["Input"], inputLayoutDesc);

    if (!LoadShaders(shaderPath, inputLayoutDesc)) return false;
    if (!CreateBlendState()) return false;

    m_effectPath = effectPath;

    return true;
}

void Cloud::Renderer::ShaderEffect::Unload()
{
    CL_ASSERT(m_vertexShader != 0, "Can't unload uninitialised vertex shader!");
    CL_ASSERT(m_pixelShader!= 0, "Can't unload uninitialised pixel shader!");

    m_inputLayout.Unload();

    if (m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = 0;
    }

    if (m_geometryShader)
    {
        m_geometryShader->Release();
        m_geometryShader = 0;
    }

    if (m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = 0;
    }

    if (m_blendState)
    {
        m_blendState->Release();
        m_blendState = 0;
    }
}

void Cloud::Renderer::ShaderEffect::Apply()
{
    CL_ASSERT_NULL(m_vertexShader);
    CL_ASSERT_NULL(m_geometryShader);
    CL_ASSERT_NULL(m_pixelShader);
    CL_ASSERT_NULL(m_blendState);

    auto immediateContext = RenderCore::Instance().GetDeviceContext();

    immediateContext->VSSetShader(m_vertexShader, 0, 0 );
    immediateContext->GSSetShader(m_geometryShader, 0, 0 );
    immediateContext->PSSetShader(m_pixelShader, 0, 0 );
    immediateContext->OMSetBlendState(m_blendState, 0, 0xffffffff);
}

CLbool Cloud::Renderer::ShaderEffect::LoadShaders(const std::string& shaderPath, const InputLayout::InputLayoutDesc& inputLayoutDesc)
{
    ID3DBlob* vertexShaderBlob = 0;
    ID3DBlob* geometryShaderBlob = 0;
    ID3DBlob* pixelShaderBlob = 0;

    if (!CompileShader(shaderPath, "VShader", "vs_5_0", vertexShaderBlob)) return false;
    if (!CompileShader(shaderPath, "GShader", "gs_5_0", geometryShaderBlob)) return false;
    if (!CompileShader(shaderPath, "PShader", "ps_5_0", pixelShaderBlob)) return false;

    auto cleanUp = [](ID3DBlob* vsBlob, ID3DBlob* gsBlob, ID3DBlob* psBlob)
    {
        if (vsBlob)
        {
            vsBlob->Release();
            vsBlob = 0;
        }

        if (gsBlob)
        {
            gsBlob->Release();
            gsBlob = 0;
        }

        if (psBlob)
        {
            psBlob->Release();
            psBlob = 0;
        }
    };

    if (!CreateVertexShader(vertexShaderBlob))
    {
        cleanUp(vertexShaderBlob, geometryShaderBlob, pixelShaderBlob);
        return false;
    }

    if (!CreateGeometryShader(geometryShaderBlob))
    {
        cleanUp(vertexShaderBlob, geometryShaderBlob, pixelShaderBlob);
        return false;
    }

    if (!CreatePixelShader(pixelShaderBlob))
    {
        cleanUp(vertexShaderBlob, geometryShaderBlob, pixelShaderBlob);
        return false;
    }

    if (!m_inputLayout.Init(vertexShaderBlob, inputLayoutDesc))
    {
        cleanUp(vertexShaderBlob, geometryShaderBlob, pixelShaderBlob);
        return false;
    }

    cleanUp(vertexShaderBlob, geometryShaderBlob, pixelShaderBlob);
    return true;
}

CLbool Cloud::Renderer::ShaderEffect::ParseInputLayout(const Json::Value& inputLayout, InputLayout::InputLayoutDesc& inputLayoutDescOutput)
{
    enum ElementVariables
    {
        Name = 0,
        Format,
    };

    if (inputLayout.size() <= 0)
    {
        return false;
    }    

    inputLayoutDescOutput.Init(inputLayout.size(), 1);
    for (CLuint i = 0; i < inputLayout.size(); ++i)
    {
        inputLayoutDescOutput.Add(InputElementDesc());
        inputLayoutDescOutput[i].name   = inputLayout[i][Name].asString();
        inputLayoutDescOutput[i].format = inputLayout[i][Format].asString();
    }

    return true;
}

CLbool Cloud::Renderer::ShaderEffect::CompileShader(const std::string& shaderPath, const std::string& entryPoint, const std::string& shaderModel, ID3DBlob*& shaderBlobOutput)
{
    CLdword shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* errorBlob = 0;
    HRESULT result = D3DX11CompileFromFile( shaderPath.c_str(),
                                            0,
                                            0,
                                            entryPoint.c_str(),
                                            shaderModel.c_str(),
                                            shaderFlags,
                                            0,
                                            0,
                                            &shaderBlobOutput,
                                            &errorBlob,
                                            0);

    if (FAILED(result) || errorBlob)
    {
        std::stringstream assertMessage;
        assertMessage << "Couldn't load " << shaderPath;

        CL_TRACE_CHANNEL("ERROR", assertMessage.str().c_str());

        if (errorBlob)
        {
            CLchar* error = static_cast<CLchar*>(errorBlob->GetBufferPointer());
            CL_TRACE_CHANNEL("ERROR", error);
        }

        CL_ASSERT_MSG(assertMessage.str().c_str());

        return false;
    }

    return true;
}

CLbool Cloud::Renderer::ShaderEffect::CreateVertexShader(ID3DBlob* vertexShaderBlob)
{
    CL_ASSERT_NULL(vertexShaderBlob);

    HRESULT result = RenderCore::Instance().GetDevice()->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), 0, &m_vertexShader);
    if (FAILED(result))
    {
        CL_ASSERT_MSG("Failed to create vertex shader!");
        return false;
    }

    return true;
}

CLbool Cloud::Renderer::ShaderEffect::CreateGeometryShader(ID3DBlob* geometryShaderBlob)
{
    CL_ASSERT_NULL(geometryShaderBlob);

    HRESULT result = RenderCore::Instance().GetDevice()->CreateGeometryShader(geometryShaderBlob->GetBufferPointer(), geometryShaderBlob->GetBufferSize(), 0, &m_geometryShader);
    if (FAILED(result))
    {
        CL_ASSERT_MSG("Failed to create geometry shader!");
        return false;
    }

    return true;
}

CLbool Cloud::Renderer::ShaderEffect::CreatePixelShader(ID3DBlob* pixelShaderBlob)
{
    CL_ASSERT_NULL(pixelShaderBlob);

    HRESULT result = RenderCore::Instance().GetDevice()->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), 0, &m_pixelShader);
    if (FAILED(result))
    {
        CL_ASSERT_MSG("Failed to create pixel shader!");
        return false;
    }

    return true;
}

CLbool Cloud::Renderer::ShaderEffect::CreateBlendState()
{
    D3D11_BLEND_DESC blendStateDesc;
    ClMemZero(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
    blendStateDesc.IndependentBlendEnable = false;
    blendStateDesc.RenderTarget[0].BlendEnable = true;
    blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    auto result = RenderCore::Instance().GetDevice()->CreateBlendState(&blendStateDesc, &m_blendState);
    if (FAILED(result))
    {
        CL_ASSERT_MSG("Failed to create blend state!");
        return false;
    }

    return true;
}