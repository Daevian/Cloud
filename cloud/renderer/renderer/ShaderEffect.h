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
            ~ShaderEffect();

            CLbool Load(const std::string& effectPath);
            void Unload();

            const InputLayout&      GetInputLayout() const      { return m_inputLayout; }
            ID3D11VertexShader*     GetVertexShader() const     { return m_vertexShader.get(); }
            ID3D11GeometryShader*   GetGeometryShader() const   { return m_geometryShader.get(); }
            ID3D11PixelShader*      GetPixelShader() const      { return m_pixelShader.get(); }
            ID3D11BlendState*       GetBlendState() const       { return m_blendState.get(); }

            const ClString&         GetPath() const             { return m_effectPath; }

        private:
            CLbool LoadShaders(const InputLayout::InputLayoutDesc& inputLayoutDesc);

            CLbool ParseInputLayout(const Json::Value& inputLayout, const Json::Value& instanceInputLayout, InputLayout::InputLayoutDesc& inputLayoutDescOutput);
            CLbool CompileShader(const ClString& shaderPath, const ClString& entryPoint, const ClString& shaderModel, ID3DBlob*& shaderBlobOutput);
            CLbool CreateVertexShader(ID3DBlob* vertexShaderBlob);
            CLbool CreateGeometryShader(ID3DBlob* geometryShaderBlob);
            CLbool CreatePixelShader(ID3DBlob* pixelShaderBlob);
            CLbool CreateBlendState();

            Dx::UniquePtr<ID3D11VertexShader>     m_vertexShader;
            Dx::UniquePtr<ID3D11GeometryShader>   m_geometryShader;
            Dx::UniquePtr<ID3D11PixelShader>      m_pixelShader;
            Dx::UniquePtr<ID3D11BlendState>       m_blendState;
            InputLayout             m_inputLayout;

            ClString                m_vsFile;
            ClString                m_gsFile;
            ClString                m_psFile;
            ClString                m_vsEntryPoint;
            ClString                m_gsEntryPoint;
            ClString                m_psEntryPoint;

            ClString                m_effectPath;
        };
    }
}

#endif // CLOUD_RENDERER_RENDEREFFECT_HEADER