#ifndef CLOUD_RENDERER_RENDEREFFECT_HEADER
#define CLOUD_RENDERER_RENDEREFFECT_HEADER

#include "InputLayout.h"

namespace Cloud
{
    namespace Renderer
    {
        class Texture;

        class ShaderEffect
        {
        public:
            ShaderEffect();

            CLbool Load(const std::string& effectPath);
            void Unload();

            InputLayout& GetInputLayout() { return m_inputLayout; }
            void Apply();

            const std::string& GetPath() const { return m_effectPath; }

        private:
            CLbool LoadShaders(const std::string& shaderPath, const InputLayout::InputLayoutDesc& inputLayoutDesc);

            CLbool ParseInputLayout(const Json::Value& inputLayout, InputLayout::InputLayoutDesc& inputLayoutDescOutput);
            CLbool CompileShader(const std::string& shaderPath, const std::string& entryPoint, const std::string& shaderModel, ID3DBlob*& shaderBlobOutput);
            CLbool CreateVertexShader(ID3DBlob* vertexShaderBlob);
            CLbool CreateGeometryShader(ID3DBlob* geometryShaderBlob);
            CLbool CreatePixelShader(ID3DBlob* pixelShaderBlob);
            CLbool CreateBlendState();

            ID3D11VertexShader* m_vertexShader;
            ID3D11GeometryShader* m_geometryShader;
            ID3D11PixelShader* m_pixelShader;
            ID3D11BlendState* m_blendState;
            InputLayout m_inputLayout;

            std::string m_effectPath;
        };
    }
}

#endif // CLOUD_RENDERER_RENDEREFFECT_HEADER