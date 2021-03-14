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

    D3D12_BLEND_DESC blendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    ParseBlendState(root["BlendState"], blendState);

    D3D12_RASTERIZER_DESC rasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    ParseRasterizerState(root["RasterizerState"], rasterizerState);

    D3D12_DEPTH_STENCIL_DESC depthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    ParseDepthStencilState(root["DepthStencilState"], depthStencilState);

    if (!LoadShaders(inputLayoutDesc, blendState, rasterizerState, depthStencilState))
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

CLbool Cloud::Renderer::ShaderEffect::LoadShaders(const InputLayout::InputLayoutDesc& inputLayoutDesc,
                                                  const D3D12_BLEND_DESC& blendState,
                                                  const D3D12_RASTERIZER_DESC& rasterizerState,
                                                  const D3D12_DEPTH_STENCIL_DESC& depthStencilState)
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
        psoDesc.InputLayout = { inputElementDescs.data(), static_cast<CLuint>(inputElementDescs.size()) };
        psoDesc.pRootSignature = RenderCore::Instance().GetRootSignature();
        psoDesc.VS = vertexShaderBlob ? CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get()) : CD3DX12_SHADER_BYTECODE(nullptr, 0);
        psoDesc.GS = geometryShaderBlob ? CD3DX12_SHADER_BYTECODE(geometryShaderBlob.Get()) : CD3DX12_SHADER_BYTECODE(nullptr, 0);
        psoDesc.PS = pixelShaderBlob ? CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get()) : CD3DX12_SHADER_BYTECODE(nullptr, 0);
        psoDesc.RasterizerState = rasterizerState;
        psoDesc.BlendState = blendState;
        psoDesc.DepthStencilState = depthStencilState;
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

D3D12_BLEND ToBlend(const std::string& str)
{
    if (str == "ZERO")              return D3D12_BLEND_ZERO;
    if (str == "ONE")               return D3D12_BLEND_ONE;
    if (str == "SRC_COLOR")         return D3D12_BLEND_SRC_COLOR;
    if (str == "INV_SRC_COLOR")     return D3D12_BLEND_INV_SRC_COLOR;
    if (str == "SRC_ALPHA")         return D3D12_BLEND_SRC_ALPHA;
    if (str == "INV_SRC_ALPHA")     return D3D12_BLEND_INV_SRC_ALPHA;
    if (str == "DEST_ALPHA")        return D3D12_BLEND_DEST_ALPHA;
    if (str == "INV_DEST_ALPHA")    return D3D12_BLEND_INV_DEST_ALPHA;
    if (str == "DEST_COLOR")        return D3D12_BLEND_DEST_COLOR;
    if (str == "INV_DEST_COLOR")    return D3D12_BLEND_INV_DEST_COLOR;
    if (str == "SRC_ALPHA_SAT")     return D3D12_BLEND_SRC_ALPHA_SAT;
    if (str == "BLEND_FACTOR")      return D3D12_BLEND_BLEND_FACTOR;
    if (str == "INV_BLEND_FACTOR")  return D3D12_BLEND_INV_BLEND_FACTOR;
    if (str == "SRC1_COLOR")        return D3D12_BLEND_SRC1_COLOR;
    if (str == "INV_SRC1_COLOR")    return D3D12_BLEND_INV_SRC1_COLOR;
    if (str == "SRC1_ALPHA")        return D3D12_BLEND_SRC1_ALPHA;
    if (str == "INV_SRC1_ALPHA")    return D3D12_BLEND_INV_SRC1_ALPHA;

    CL_ASSERT_MSG("unknown value");
    return D3D12_BLEND_ZERO;
}

D3D12_BLEND_OP ToBlendOp(const std::string& str)
{
    if (str == "ADD")               return D3D12_BLEND_OP_ADD;
    if (str == "SUBTRACT")          return D3D12_BLEND_OP_SUBTRACT;
    if (str == "REV_SUBTRACT")      return D3D12_BLEND_OP_REV_SUBTRACT;
    if (str == "MIN")               return D3D12_BLEND_OP_MIN;
    if (str == "MAX")               return D3D12_BLEND_OP_MAX;

    CL_ASSERT_MSG("unknown value");
    return D3D12_BLEND_OP_ADD;
}

D3D12_FILL_MODE ToFillMode(const std::string& str)
{
    if (str == "WIREFRAME")         return D3D12_FILL_MODE_WIREFRAME;
    if (str == "SOLID")             return D3D12_FILL_MODE_SOLID;

    CL_ASSERT_MSG("unknown value");
    return D3D12_FILL_MODE_SOLID;
}

D3D12_CULL_MODE ToCullMode(const std::string& str)
{
    if (str == "NONE")              return D3D12_CULL_MODE_NONE;
    if (str == "FRONT")             return D3D12_CULL_MODE_FRONT;
    if (str == "BACK")              return D3D12_CULL_MODE_BACK;

    CL_ASSERT_MSG("unknown value");
    return D3D12_CULL_MODE_NONE;
}

D3D12_DEPTH_WRITE_MASK ToDepthWriteMask(const std::string& str)
{
    if (str == "ZERO")              return D3D12_DEPTH_WRITE_MASK_ZERO;
    if (str == "ALL")               return D3D12_DEPTH_WRITE_MASK_ALL;

    CL_ASSERT_MSG("unknown value");
    return D3D12_DEPTH_WRITE_MASK_ZERO;
}

D3D12_COMPARISON_FUNC ToComparisonFunc(const std::string& str)
{
    if (str == "NEVER")             return D3D12_COMPARISON_FUNC_NEVER;
    if (str == "LESS")              return D3D12_COMPARISON_FUNC_LESS;
    if (str == "EQUAL")             return D3D12_COMPARISON_FUNC_EQUAL;
    if (str == "LESS_EQUAL")        return D3D12_COMPARISON_FUNC_LESS_EQUAL;
    if (str == "GREATER")           return D3D12_COMPARISON_FUNC_GREATER;
    if (str == "NOT_EQUAL")         return D3D12_COMPARISON_FUNC_NOT_EQUAL;
    if (str == "GREATER_EQUAL")     return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
    if (str == "ALWAYS")            return D3D12_COMPARISON_FUNC_ALWAYS;

    CL_ASSERT_MSG("unknown value");
    return D3D12_COMPARISON_FUNC_NEVER;
}

D3D12_STENCIL_OP ToStencilOp(const std::string& str)
{
    if (str == "KEEP")              return D3D12_STENCIL_OP_KEEP;
    if (str == "ZERO")              return D3D12_STENCIL_OP_ZERO;
    if (str == "REPLACE")           return D3D12_STENCIL_OP_REPLACE;
    if (str == "INCR_SAT")          return D3D12_STENCIL_OP_INCR_SAT;
    if (str == "DECR_SAT")          return D3D12_STENCIL_OP_DECR_SAT;
    if (str == "INVERT")            return D3D12_STENCIL_OP_INVERT;
    if (str == "INCR")              return D3D12_STENCIL_OP_INCR;
    if (str == "DECR")              return D3D12_STENCIL_OP_DECR;

    CL_ASSERT_MSG("unknown value");
    return D3D12_STENCIL_OP_KEEP;
}

CLbool Cloud::Renderer::ShaderEffect::ParseBlendState(const Json::Value& blendState, D3D12_BLEND_DESC& blendStateDesc)
{
    if (blendState["BlendEnable"].isBool())
    {
        blendStateDesc.RenderTarget[0].BlendEnable = blendState["BlendEnable"].asBool();
    }

    if (blendState["SrcBlend"].isString())
    {
        blendStateDesc.RenderTarget[0].SrcBlend = ToBlend(blendState["SrcBlend"].asString());
    }

    if (blendState["DestBlend"].isString())
    {
        blendStateDesc.RenderTarget[0].DestBlend = ToBlend(blendState["DestBlend"].asString());
    }

    if (blendState["BlendOp"].isString())
    {
        blendStateDesc.RenderTarget[0].BlendOp = ToBlendOp(blendState["BlendOp"].asString());
    }

    if (blendState["SrcBlendAlpha"].isString())
    {
        blendStateDesc.RenderTarget[0].SrcBlendAlpha = ToBlend(blendState["SrcBlendAlpha"].asString());
    }

    if (blendState["DestBlendAlpha"].isString())
    {
        blendStateDesc.RenderTarget[0].DestBlendAlpha = ToBlend(blendState["DestBlendAlpha"].asString());
    }

    if (blendState["BlendOpAlpha"].isString())
    {
        blendStateDesc.RenderTarget[0].BlendOpAlpha = ToBlendOp(blendState["BlendOpAlpha"].asString());
    }
    
    return true;
}

CLbool Cloud::Renderer::ShaderEffect::ParseRasterizerState(const Json::Value& rasterizerState, D3D12_RASTERIZER_DESC& rasterizerStateDesc)
{
    if (rasterizerState["FillMode"].isString())
    {
        rasterizerStateDesc.FillMode = ToFillMode(rasterizerState["FillMode"].asString());
    }

    if (rasterizerState["CullMode"].isString())
    {
        rasterizerStateDesc.CullMode = ToCullMode(rasterizerState["CullMode"].asString());
    }

    if (rasterizerState["DepthClipEnable"].isBool())
    {
        rasterizerStateDesc.DepthClipEnable = rasterizerState["DepthClipEnable"].asBool();
    }

    return true;
}

CLbool Cloud::Renderer::ShaderEffect::ParseDepthStencilState(const Json::Value& depthStencilState, D3D12_DEPTH_STENCIL_DESC& depthStencilStateDesc)
{
    if (depthStencilState["DepthEnable"].isBool())
    {
        depthStencilStateDesc.DepthEnable = depthStencilState["DepthEnable"].asBool();
    }

    if (depthStencilState["DepthWriteMask"].isString())
    {
        depthStencilStateDesc.DepthWriteMask = ToDepthWriteMask(depthStencilState["DepthWriteMask"].asString());
    }

    if (depthStencilState["DepthFunc"].isString())
    {
        depthStencilStateDesc.DepthFunc = ToComparisonFunc(depthStencilState["DepthFunc"].asString());
    }

    if (depthStencilState["StencilEnable"].isBool())
    {
        depthStencilStateDesc.StencilEnable = depthStencilState["StencilEnable"].asBool();
    }

    auto parseStencilStateFace = [](const Json::Value& face, D3D12_DEPTH_STENCILOP_DESC& faceDesc)
    {
        if (!face.isNull())
        {
            if (face["StencilFailOp"].isString())
            {
                faceDesc.StencilFailOp = ToStencilOp(face["StencilFailOp"].asString());
            }

            if (face["StencilDepthFailOp"].isString())
            {
                faceDesc.StencilDepthFailOp = ToStencilOp(face["StencilDepthFailOp"].asString());
            }

            if (face["StencilPassOp"].isString())
            {
                faceDesc.StencilPassOp = ToStencilOp(face["StencilPassOp"].asString());
            }

            if (face["StencilFunc"].isString())
            {
                faceDesc.StencilFunc = ToComparisonFunc(face["StencilFunc"].asString());
            }
        }
    };

    parseStencilStateFace(depthStencilState["FrontFace"], depthStencilStateDesc.FrontFace);
    parseStencilStateFace(depthStencilState["BackFace"], depthStencilStateDesc.BackFace);

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