#ifndef CLOUD_RENDERER_INPUTLAYOUT_HEADER
#define CLOUD_RENDERER_INPUTLAYOUT_HEADER

#include "GfxFormat.h"

namespace Cloud
{
    namespace Renderer
    {
        struct InputElementDesc
        {
            std::string name;
            GfxFormat   format;
            CLint       semanticIndex;
            CLint       byteOffset;
            CLint       instanceDataStepRate;
            CLbool      isInstanceData;
        };

        class InputLayout
        {
        public:
            typedef Utils::DynamicArray<InputElementDesc> InputLayoutDesc;

            InputLayout();

            CLbool Init(ID3DBlob* vertexShaderBlob, const InputLayoutDesc& elementDescs);
            void Unload();

#ifdef USE_DIRECTX12
            std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElementDescs() { return m_inputElementDescs; }
#else
            ID3D11InputLayout* GetInputLayout() const { return m_inputLayout.get(); }
#endif

        private:
#ifdef USE_DIRECTX12
            std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputElementDescs;
#else
            Dx::UniquePtr<ID3D11InputLayout> m_inputLayout;
#endif
        };
    }
}

#endif // CLOUD_RENDERER_INPUTLAYOUT_HEADER