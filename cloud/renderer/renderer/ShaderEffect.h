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

            bool Load(const std::string& effectPath);
            void Unload();

            const InputLayout&      GetInputLayout() const      { return m_inputLayout; }

#ifdef USE_DIRECTX12
            ID3D12PipelineState* GetPipelineState() const { return m_pipelineState.Get(); }
#else
            ID3D11VertexShader*     GetVertexShader() const     { return m_vertexShader.get(); }
            ID3D11GeometryShader*   GetGeometryShader() const   { return m_geometryShader.get(); }
            ID3D11PixelShader*      GetPixelShader() const      { return m_pixelShader.get(); }
            ID3D11BlendState*       GetBlendState() const       { return m_blendState.get(); }
#endif

            const Cloud::String&         GetPath() const             { return m_effectPath; }

        private:
            bool LoadShaders(const InputLayout::InputLayoutDesc& inputLayoutDesc,
                               const D3D12_BLEND_DESC& blendState,
                               const D3D12_RASTERIZER_DESC& rasterizerState,
                               const D3D12_DEPTH_STENCIL_DESC& depthStencilState);

            bool ParseInputLayout(const Json::Value& inputLayout, const Json::Value& instanceInputLayout, InputLayout::InputLayoutDesc& inputLayoutDescOutput);
            bool ParseBlendState(const Json::Value& blendState, D3D12_BLEND_DESC& blendStateDesc);
            bool ParseRasterizerState(const Json::Value& rasterizerState, D3D12_RASTERIZER_DESC& rasterizerStateDesc);
            bool ParseDepthStencilState(const Json::Value& depthStencilState, D3D12_DEPTH_STENCIL_DESC& depthStencilStateDesc);
#ifdef USE_DIRECTX12
            ComPtr<ID3DBlob> CompileShader(const Cloud::String& shaderPath, const Cloud::String& entryPoint, const Cloud::String& shaderModel);
#else
            bool CompileShader(const Cloud::String& shaderPath, const Cloud::String& entryPoint, const Cloud::String& shaderModel, ID3DBlob*& shaderBlobOutput);
            bool CreateVertexShader(ID3DBlob* vertexShaderBlob);
            bool CreateGeometryShader(ID3DBlob* geometryShaderBlob);
            bool CreatePixelShader(ID3DBlob* pixelShaderBlob);
#endif
            bool CreateBlendState();

#ifdef USE_DIRECTX12
            ComPtr<ID3D12PipelineState> m_pipelineState;
#else
            Dx::UniquePtr<ID3D11VertexShader>     m_vertexShader;
            Dx::UniquePtr<ID3D11GeometryShader>   m_geometryShader;
            Dx::UniquePtr<ID3D11PixelShader>      m_pixelShader;
            Dx::UniquePtr<ID3D11BlendState>       m_blendState;
#endif
            InputLayout             m_inputLayout;

            Cloud::String                m_vsFile;
            Cloud::String                m_gsFile;
            Cloud::String                m_psFile;
            Cloud::String                m_vsEntryPoint;
            Cloud::String                m_gsEntryPoint;
            Cloud::String                m_psEntryPoint;

            Cloud::String                m_effectPath;
        };
    }
}

#endif // CLOUD_RENDERER_RENDEREFFECT_HEADER