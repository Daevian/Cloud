#include "stdafx.h"
#include "InputLayout.h"

#include "RenderCore.h"

Cloud::Renderer::InputLayout::InputLayout()
{
}

CLbool Cloud::Renderer::InputLayout::Init(ID3DBlob* vertexShaderBlob, const InputLayoutDesc& elementDescs)
{
#ifdef USE_DIRECTX12
    CL_UNUSED(vertexShaderBlob);

    m_inputElementDescs.reserve(elementDescs.Count());

    for (CLint i = 0; i < elementDescs.Count(); ++i)
    {
        auto&& elementDesc = elementDescs[i];

        m_inputElementDescs.emplace_back();

        auto& inputElement = m_inputElementDescs.back();
        inputElement.SemanticName = elementDesc.name.c_str();
        inputElement.SemanticIndex = elementDesc.semanticIndex;
        inputElement.Format = (DXGI_FORMAT)elementDesc.format;
        inputElement.InputSlot = elementDesc.isInstanceData ? 1 : 0;
        inputElement.AlignedByteOffset = elementDesc.byteOffset;
        inputElement.InputSlotClass = elementDesc.isInstanceData ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        inputElement.InstanceDataStepRate = elementDesc.instanceDataStepRate;
    }

    return true;
#else
    CL_ASSERT_NULL(vertexShaderBlob);

    Utils::DynamicArray<D3D11_INPUT_ELEMENT_DESC> inputElements(elementDescs.Count(), 1);

    for (CLint i = 0; i < elementDescs.Count(); ++i)
    {
        const InputElementDesc& elementDesc = elementDescs[i];

        D3D11_INPUT_ELEMENT_DESC inputElement;
        inputElement.SemanticName = elementDesc.name.c_str();
        inputElement.SemanticIndex = elementDesc.semanticIndex;
        inputElement.Format = (DXGI_FORMAT)elementDesc.format;
        inputElement.InputSlot = elementDesc.isInstanceData ? 1 : 0;
        inputElement.AlignedByteOffset = elementDesc.byteOffset;
        inputElement.InputSlotClass = elementDesc.isInstanceData ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
        inputElement.InstanceDataStepRate = elementDesc.instanceDataStepRate;

        inputElements.Add(inputElement);
    }

    ID3D11InputLayout* dxInputLayout;
    HRESULT result = RenderCore::Instance().GetDevice()->CreateInputLayout( reinterpret_cast<D3D11_INPUT_ELEMENT_DESC*>(&inputElements[0]),
                                                                            inputElements.Count(),
                                                                            vertexShaderBlob->GetBufferPointer(),
                                                                            vertexShaderBlob->GetBufferSize(),
                                                                            &dxInputLayout);

    if (FAILED(result))
    {
        CL_ASSERT_MSG("Couldn't create input layout!");
        return false;
    }

    m_inputLayout = Dx::MakeUnique(dxInputLayout);

    return true;
#endif
}

void Cloud::Renderer::InputLayout::Unload()
{
#ifdef USE_DIRECTX12
#else
    CL_ASSERT(m_inputLayout != 0, "Can't unload uninitialised input layout!");

    m_inputLayout = nullptr;
#endif
}
