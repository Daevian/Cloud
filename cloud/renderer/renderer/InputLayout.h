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

            ID3D11InputLayout* GetInputLayout() const { return m_inputLayout.get(); }

        private:
            Dx::UniquePtr<ID3D11InputLayout> m_inputLayout;
        };
    }
}

#endif // CLOUD_RENDERER_INPUTLAYOUT_HEADER