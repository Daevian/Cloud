#include "stdafx.h"
#include "ShaderEffect.h"

#include "RenderCore.h"
#include "Texture.h"
#include <d3dcompiler.h>

Cloud::Renderer::ShaderEffect::ShaderEffect()
{
}

Cloud::Renderer::ShaderEffect::~ShaderEffect()
{
}

CLbool Cloud::Renderer::ShaderEffect::Load(const std::string& effectPath)
{
    m_effectPath = effectPath;

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

    m_vsFile = root["VertexShader"]["File"].asString();
    m_vsEntryPoint = root["VertexShader"]["EntryPoint"].asString();
    m_gsFile = root["GeometryShader"]["File"].asString();
    m_gsEntryPoint = root["GeometryShader"]["EntryPoint"].asString();
    m_psFile = root["PixelShader"]["File"].asString();
    m_psEntryPoint = root["PixelShader"]["EntryPoint"].asString();

    InputLayout::InputLayoutDesc inputLayoutDesc;
    ParseInputLayout(root["Input"], root["InstanceInput"], inputLayoutDesc);

    if (!LoadShaders(inputLayoutDesc))
        return false;

#ifdef USE_DIRECTX12

    return true;
#else

    CL_ASSERT(m_vertexShader == 0, ("vertex shader in " + effectPath + " already loaded!").c_str());
    CL_ASSERT(m_geometryShader == 0, ("geometry shader in " + effectPath + " already loaded!").c_str());
    CL_ASSERT(m_pixelShader == 0, ("pixel shader in " + effectPath + " already loaded!").c_str());
    
    if (!CreateBlendState())
        return false;

    return true;
#endif
}

void Cloud::Renderer::ShaderEffect::Unload()
{
#ifdef USE_DIRECTX12
#else
    CL_ASSERT(m_vertexShader != 0, "Can't unload uninitialised vertex shader!");
    CL_ASSERT(m_pixelShader!= 0, "Can't unload uninitialised pixel shader!");

    m_inputLayout.Unload();

    m_vertexShader = nullptr;
    m_geometryShader = nullptr;
    m_pixelShader = nullptr;
    m_blendState = nullptr;
#endif
}

CLbool Cloud::Renderer::ShaderEffect::LoadShaders(const InputLayout::InputLayoutDesc& inputLayoutDesc)
{
#ifdef USE_DIRECTX12
    ComPtr<ID3DBlob> vertexShaderBlob;
    ComPtr<ID3DBlob> geometryShaderBlob;
    ComPtr<ID3DBlob> pixelShaderBlob;

    if (!m_vsFile.empty() && !m_vsEntryPoint.empty())
    {
        vertexShaderBlob = CompileShader(m_vsFile, m_vsEntryPoint, "vs_5_0");
        if (!vertexShaderBlob)
        {
            return false;
        }
    }

    if (!m_gsFile.empty() && !m_gsEntryPoint.empty())
    {
        geometryShaderBlob = CompileShader(m_gsFile, m_gsEntryPoint, "gs_5_0");
        if (!geometryShaderBlob)
        {
            return false;
        }
    }

    if (!m_psFile.empty() && !m_psEntryPoint.empty())
    {
        pixelShaderBlob = CompileShader(m_psFile, m_psEntryPoint, "ps_5_0");
        if (!pixelShaderBlob)
        {
            return false;
        }
    }

    if (!m_inputLayout.Init(vertexShaderBlob.Get(), inputLayoutDesc))
    {
        return false;
    }

    // create PSO
    {
        auto& inputElementDescs = m_inputLayout.GetInputElementDescs();
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs.data(), gsl::narrow_cast<CLuint>(inputElementDescs.size()) };
        psoDesc.pRootSignature = RenderCore::Instance().GetRootSignature();
        psoDesc.VS = vertexShaderBlob ? CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get()) : CD3DX12_SHADER_BYTECODE(nullptr, 0);
        psoDesc.GS = geometryShaderBlob ? CD3DX12_SHADER_BYTECODE(geometryShaderBlob.Get()) : CD3DX12_SHADER_BYTECODE(nullptr, 0);
        psoDesc.PS = pixelShaderBlob ? CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get()) : CD3DX12_SHADER_BYTECODE(nullptr, 0);
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        //psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.DepthEnable = TRUE;
        psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;

        if (FAILED(RenderCore::Instance().GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState))))
        {
            CL_ASSERT_MSG("Failed to create PSO!");
            return false;
        }
    }

    return true;

#else
    Dx::UniquePtr<ID3DBlob> vertexShaderBlob;
    Dx::UniquePtr<ID3DBlob> geometryShaderBlob;
    Dx::UniquePtr<ID3DBlob> pixelShaderBlob;

    if (!m_vsFile.empty() && !m_vsEntryPoint.empty())
    {
        ID3DBlob* dxObj;
        if (!CompileShader(m_vsFile, m_vsEntryPoint, "vs_5_0", dxObj))
        {
            return false;
        }

        vertexShaderBlob = Dx::MakeUnique(dxObj);
    }

    if (!m_gsFile.empty() && !m_gsEntryPoint.empty())
    {
        ID3DBlob* dxObj;
        if (!CompileShader(m_gsFile, m_gsEntryPoint, "gs_5_0", dxObj))
        {
            return false;
        }

        geometryShaderBlob = Dx::MakeUnique(dxObj);
    }

    if (!m_psFile.empty() && !m_psEntryPoint.empty())
    {
        ID3DBlob* dxObj;
        if (!CompileShader(m_psFile, m_psEntryPoint, "ps_5_0", dxObj))
        {
            return false;
        }

        pixelShaderBlob = Dx::MakeUnique(dxObj);
    }

    if (vertexShaderBlob)
    {
        if (!CreateVertexShader(vertexShaderBlob.get()))
        {
            return false;
        }
    }
    
    if (geometryShaderBlob)
    {
        if (!CreateGeometryShader(geometryShaderBlob.get()))
        {
            return false;
        }
    }

    if (pixelShaderBlob)
    {
        if (!CreatePixelShader(pixelShaderBlob.get()))
        {
            return false;
        }
    }

    if (!m_inputLayout.Init(vertexShaderBlob.get(), inputLayoutDesc))
    {
        return false;
    }

    return true;
#endif
}

#ifdef USE_DIRECTX12

CLbool Cloud::Renderer::ShaderEffect::ParseInputLayout(const Json::Value& inputLayout, const Json::Value& instanceInputLayout, InputLayout::InputLayoutDesc& inputLayoutDescOutput)
{
    enum ElementVariables
    {
        Name = 0,
        Format,
        SemanticIndex,
        InstanceDataStepRate,
    };

    if (inputLayout.size() <= 0 && instanceInputLayout.size() <= 0)
    {
        return false;
    }

    inputLayoutDescOutput.Init(inputLayout.size() + instanceInputLayout.size(), 1);

    CLint elementByteOffset = 0;
    for (CLuint i = 0; i < inputLayout.size(); ++i)
    {
        inputLayoutDescOutput.Add(InputElementDesc());
        inputLayoutDescOutput[i].name = inputLayout[i][Name].asString();
        inputLayoutDescOutput[i].format = GetFormat(inputLayout[i][Format].asString());
        inputLayoutDescOutput[i].semanticIndex = inputLayout[i][SemanticIndex].asInt();
        inputLayoutDescOutput[i].byteOffset = elementByteOffset;
        inputLayoutDescOutput[i].instanceDataStepRate = 0;
        inputLayoutDescOutput[i].isInstanceData = false;

        elementByteOffset += GetFormatSize(inputLayoutDescOutput[i].format);
    }

    elementByteOffset = 0;
    for (CLuint targetIndex = inputLayoutDescOutput.Count(), sourceIndex = 0; sourceIndex < instanceInputLayout.size(); ++targetIndex, ++sourceIndex)
    {
        inputLayoutDescOutput.Add(InputElementDesc());
        inputLayoutDescOutput[targetIndex].name = instanceInputLayout[sourceIndex][Name].asString();
        inputLayoutDescOutput[targetIndex].format = GetFormat(instanceInputLayout[sourceIndex][Format].asString());
        inputLayoutDescOutput[targetIndex].semanticIndex = instanceInputLayout[sourceIndex][SemanticIndex].asInt();
        inputLayoutDescOutput[targetIndex].byteOffset = elementByteOffset;
        inputLayoutDescOutput[targetIndex].instanceDataStepRate = instanceInputLayout[sourceIndex][InstanceDataStepRate].asInt();
        inputLayoutDescOutput[targetIndex].isInstanceData = true;

        elementByteOffset += GetFormatSize(inputLayoutDescOutput[targetIndex].format);
    }

    return true;
}

Cloud::ComPtr<ID3DBlob> Cloud::Renderer::ShaderEffect::CompileShader(const ClString& shaderPath, const ClString& entryPoint, const ClString& shaderModel)
{
    CLdword shaderFlags = 0;//D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3DCOMPILE_DEBUG;
    shaderFlags |= D3DCOMPILE_PREFER_FLOW_CONTROL;
    shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    Cloud::ComPtr<ID3DBlob> shader;

    CL_UNUSED(shaderModel);
    CL_UNUSED(entryPoint);
    ID3DBlob* errorBlob = 0;
    HRESULT result = D3DCompileFromFile(
        ClWString(shaderPath.begin(), shaderPath.end()).c_str(),
        nullptr,
        nullptr,
        entryPoint.c_str(),
        shaderModel.c_str(),
        shaderFlags,
        0,
        &shader,
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
        }

        CL_ASSERT_MSG(assertMessage.str().c_str());

        return nullptr;
    }

    return shader;
}

#else


CLbool Cloud::Renderer::ShaderEffect::CompileShader(const ClString& /*shaderPath*/, const ClString& /*entryPoint*/, const ClString& /*shaderModel*/, ID3DBlob*& /*shaderBlobOutput*/)
{
    CLdword shaderFlags = 0;//D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3DCOMPILE_DEBUG;
    shaderFlags |= D3DCOMPILE_PREFER_FLOW_CONTROL;
    shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    CL_UNUSED(shaderBlobOutput);
    CL_UNUSED(shaderModel);
    CL_UNUSED(entryPoint);
    ID3DBlob* errorBlob = 0;
    HRESULT result = D3DCompileFromFile(
        ClWString(shaderPath.begin(), shaderPath.end()).c_str(),
        nullptr,
        nullptr,
        entryPoint.c_str(),
        shaderModel.c_str(),
        shaderFlags,
        0,
        &shaderBlobOutput,
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
        }

        CL_ASSERT_MSG(assertMessage.str().c_str());

        return false;
    }

    return true;
}

CLbool Cloud::Renderer::ShaderEffect::CreateVertexShader(ID3DBlob* vertexShaderBlob)
{
    CL_ASSERT_NULL(vertexShaderBlob);

    ID3D11VertexShader* dxObj;
    HRESULT result = RenderCore::Instance().GetDevice()->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), 0, &dxObj);
    if (FAILED(result))
    {
        CL_ASSERT_MSG("Failed to create vertex shader!");
        return false;
    }

    m_vertexShader = Dx::MakeUnique(dxObj);

    return true;
}

CLbool Cloud::Renderer::ShaderEffect::CreateGeometryShader(ID3DBlob* geometryShaderBlob)
{
    CL_ASSERT_NULL(geometryShaderBlob);

    ID3D11GeometryShader* dxObj;
    HRESULT result = RenderCore::Instance().GetDevice()->CreateGeometryShader(geometryShaderBlob->GetBufferPointer(), geometryShaderBlob->GetBufferSize(), 0, &dxObj);
    if (FAILED(result))
    {
        CL_ASSERT_MSG("Failed to create geometry shader!");
        return false;
    }

    m_geometryShader = Dx::MakeUnique(dxObj);

    return true;
}

CLbool Cloud::Renderer::ShaderEffect::CreatePixelShader(ID3DBlob* pixelShaderBlob)
{
    CL_ASSERT_NULL(pixelShaderBlob);

    ID3D11PixelShader* dxObj;
    HRESULT result = RenderCore::Instance().GetDevice()->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), 0, &dxObj);
    if (FAILED(result))
    {
        CL_ASSERT_MSG("Failed to create pixel shader!");
        return false;
    }

    m_pixelShader = Dx::MakeUnique(dxObj);

    return true;
}
#endif

CLbool Cloud::Renderer::ShaderEffect::CreateBlendState()
{
#ifdef USE_DIRECTX12
    return false;
#else
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

    ID3D11BlendState* dxObj;
    auto result = RenderCore::Instance().GetDevice()->CreateBlendState(&blendStateDesc, &dxObj);
    if (FAILED(result))
    {
        CL_ASSERT_MSG("Failed to create blend state!");
        return false;
    }

    m_blendState = Dx::MakeUnique(dxObj);

    return true;
#endif
}