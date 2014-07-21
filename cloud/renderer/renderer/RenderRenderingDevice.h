#ifndef CLOUD_RENDERER_RENDERING_DEVICE_HEADER
#define CLOUD_RENDERER_RENDERING_DEVICE_HEADER

#include "GfxCommon.h"
#include "GfxFormat.h"

namespace Cloud
{
    namespace Renderer
    {
        class ShaderEffect;
        class GfxComputeShader;
        class VertexBuffer;
        class IndexBuffer;
        class GfxInstanceBuffer;
        class GfxConstantBuffer;
        class GfxTexture;
        class GfxBuffer;
        class Texture;
        class InputLayout;

        struct GfxBufferMapDesc
        {
            GfxBuffer*  buffer;
            CLuint      subresource;
            D3D11_MAP   mapType;
            CLuint      mapFlags;
        };

        struct GfxMappedResource
        {
            void*   data;
            CLuint  rowPitch;
            CLuint  depthPitch;
        };

        class RenderingDevice
        {
        public:
            RenderingDevice();

            void SetEffect(ShaderEffect* effect);
            void SetInputLayout(ID3D11InputLayout* inputLayout);
            void SetVertexShader(ID3D11VertexShader* vertexShader);
            void SetGeometryShader(ID3D11GeometryShader* geometryShader);
            void SetPixelShader(ID3D11PixelShader* pixelShader);
            void SetComputeShader(GfxComputeShader* shader);
            void SetBlendState(ID3D11BlendState* blendState);

            void SetVertexBuffer(VertexBuffer* vertexBuffer);
            void SetIndexBuffer(IndexBuffer* indexBuffer);
            void SetInstanceBuffer(GfxInstanceBuffer* vertexBuffer);
            void SetConstantBuffer(GfxConstantBuffer* constantBuffer, CLuint slot);
            void SetPrimitiveTopology(GfxPrimitiveTopology topology);

            void SetTexturePS(GfxTexture* texture);
            void SetBufferCS(GfxBuffer* buffer, CLuint slot);
            void SetConstantBufferCS(GfxBuffer* buffer, CLuint slot);
            void SetUnorderedAccessView(GfxBuffer* buffer, CLuint slot);
            void SetSamplerState(ID3D11SamplerState* samplerState, CLuint slot);

            void SetShaderResourceVS(ID3D11ShaderResourceView* srv, CLuint slot);
            void SetShaderResourcePS(ID3D11ShaderResourceView* srv, CLuint slot);
            void SetShaderResourceCS(ID3D11ShaderResourceView* srv, CLuint slot);

            void Copy(GfxBuffer* source, GfxBuffer* dest);
            CLbool Map(const GfxBufferMapDesc& desc, GfxMappedResource& mappedResource);
            void Unmap(const GfxBufferMapDesc& desc);
            void UpdateSubresource(GfxBuffer* buffer, void* data);

            void Draw(CLint vertexCount = -1);
            void DrawInstanced(CLuint instanceCount, CLint vertexCount = -1);
            void DrawIndexed(CLint indexCount = -1);
            void DrawIndexedInstanced(CLuint instanceCount, CLint indexCount = -1);
            void Dispatch(const CLuint threadGroupCountX, const CLuint threadGroupCountY, const CLuint threadGroupCountZ);

        private:
            CLuint          m_vertexCount;
            CLuint          m_indexCount;
        };
    }
}

#endif // CLOUD_RENDERER_RENDERING_DEVICE_HEADER