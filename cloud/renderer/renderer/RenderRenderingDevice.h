#ifndef CLOUD_RENDERER_RENDERING_DEVICE_HEADER
#define CLOUD_RENDERER_RENDERING_DEVICE_HEADER

#include "GfxCommon.h"
#include "GfxFormat.h"

namespace Cloud
{
namespace Renderer
{
    class ShaderEffect;
    class VertexBuffer;
    class IndexBuffer;
    class GfxInstanceBuffer;
    class GfxConstantBuffer;
    class Texture;
    class InputLayout;

    class RenderingDevice
    {
    public:
        RenderingDevice();

        CLbool Init();

        void SetEffect(ShaderEffect* effect);
        void SetInputLayout(ID3D11InputLayout* inputLayout);
        void SetVertexShader(ID3D11VertexShader* vertexShader);
        void SetGeometryShader(ID3D11GeometryShader* geometryShader);
        void SetPixelShader(ID3D11PixelShader* pixelShader);
        void SetBlendState(ID3D11BlendState* blendState);

        void SetVertexBuffer(VertexBuffer* vertexBuffer);
        void SetIndexBuffer(IndexBuffer* indexBuffer);
        void SetInstanceBuffer(GfxInstanceBuffer* vertexBuffer);
        void SetConstantBuffer(GfxConstantBuffer* constantBuffer, CLuint slot);
        void SetPrimitiveTopology(GfxPrimitiveTopology topology);

        void SetTexture(Texture* texture);
        void SetShaderResource(ID3D11ShaderResourceView* srv, CLuint slot);
        void SetSamplerState(ID3D11SamplerState* samplerState, CLuint slot);

        void Draw(CLint vertexCount = -1);
        void DrawInstanced(CLuint instanceCount, CLint vertexCount = -1);
        void DrawIndexed(CLint indexCount = -1);
        void DrawIndexedInstanced(CLuint instanceCount, CLint indexCount = -1);

    private:
        CLuint          m_vertexCount;
        CLuint          m_indexCount;
    };
}
}

#endif // CLOUD_RENDERER_RENDERING_DEVICE_HEADER