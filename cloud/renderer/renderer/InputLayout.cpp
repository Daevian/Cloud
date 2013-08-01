#include "stdafx.h"
#include "InputLayout.h"

#include "RenderCore.h"

Cloud::Renderer::InputLayout::InputLayout()
:m_inputLayout(0)
{
}

CLbool Cloud::Renderer::InputLayout::Init(ID3DBlob* vertexShaderBlob, const InputLayoutDesc& elementDescs)
{
    CL_ASSERT_NULL(vertexShaderBlob);

    Utils::DynamicArray<D3D11_INPUT_ELEMENT_DESC> inputElements(elementDescs.Count(), 1);

    CLint elementByteOffset = 0;
    for (CLint i = 0; i < elementDescs.Count(); ++i)
    {
        const InputElementDesc& elementDesc = elementDescs[i];

        D3D11_INPUT_ELEMENT_DESC inputElement;
        inputElement.SemanticName = elementDesc.name.c_str();
        inputElement.SemanticIndex = 0;
        inputElement.Format = GetFormat(elementDesc.format);
        inputElement.InputSlot = 0;
        inputElement.AlignedByteOffset = elementByteOffset;
        inputElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputElement.InstanceDataStepRate = 0;

        inputElements.Add(inputElement);

        elementByteOffset += GetFormatSize(inputElement.Format);
    }

    HRESULT result = RenderCore::Instance().GetDevice()->CreateInputLayout( reinterpret_cast<D3D11_INPUT_ELEMENT_DESC*>(&inputElements[0]),
                                                                            inputElements.Count(),
                                                                            vertexShaderBlob->GetBufferPointer(),
                                                                            vertexShaderBlob->GetBufferSize(),
                                                                            &m_inputLayout);

    if (FAILED(result))
    {
        CL_ASSERT_MSG("Couldn't create input layout!");
        return false;
    }

    return true;
}

void Cloud::Renderer::InputLayout::Unload()
{
    CL_ASSERT(m_inputLayout != 0, "Can't unload uninitialised input layout!");

    if (m_inputLayout)
    {
        m_inputLayout->Release();
        m_inputLayout = 0;
    }
}

void Cloud::Renderer::InputLayout::GPUSetInputLayout()
{
    RenderCore::Instance().GetDeviceContext()->IASetInputLayout(m_inputLayout);
}

DXGI_FORMAT Cloud::Renderer::InputLayout::GetFormat(const std::string& formatName)
{
    if (formatName == "R32G32_FLOAT")
    {
        return DXGI_FORMAT_R32G32_FLOAT;
    }
    
    return DXGI_FORMAT_UNKNOWN;
}

CLuint Cloud::Renderer::InputLayout::GetFormatSize(DXGI_FORMAT format)
{
    switch (format)
    {
    case DXGI_FORMAT_R32G32_FLOAT:
        return 8;

    case DXGI_FORMAT_UNKNOWN:
    default:
        return 0;
    }
}