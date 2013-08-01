#ifndef CLOUD_RENDERER_INPUTLAYOUT_HEADER
#define CLOUD_RENDERER_INPUTLAYOUT_HEADER

namespace Cloud
{
    namespace Renderer
    {
        struct InputElementDesc
        {
            std::string name;
            std::string format;
            CLint       byteOffset;
        };

        class InputLayout
        {
        public:
            typedef Utils::DynamicArray<InputElementDesc> InputLayoutDesc;

            InputLayout();

            CLbool Init(ID3DBlob* vertexShaderBlob, const InputLayoutDesc& elementDescs);
            void Unload();

            void GPUSetInputLayout();

        private:
            DXGI_FORMAT GetFormat(const std::string& formatName);
            CLuint      GetFormatSize(DXGI_FORMAT format);

            ID3D11InputLayout* m_inputLayout; //4
        }; //4
    }
}

#endif // CLOUD_RENDERER_INPUTLAYOUT_HEADER