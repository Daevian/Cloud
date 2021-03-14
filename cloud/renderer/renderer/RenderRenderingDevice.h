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
            uint      subresource;
#ifdef USE_DIRECTX12
#else
            D3D11_MAP   mapType;
#endif
            uint      mapFlags;
        };

        struct GfxMappedResource
        {
            void*   data;
            uint  rowPitch;
            uint  depthPitch;
        };

        class RenderingDevice
        {
        public:
            RenderingDevice();

#ifdef USE_DIRECTX12
#else
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
            void SetConstantBuffer(GfxConstantBuffer* constantBuffer, uint slot);
            void SetPrimitiveTopology(GfxPrimitiveTopology topology);

            void SetTexturePS(GfxTexture* texture);
            void SetBufferCS(GfxBuffer* buffer, uint slot);
            void SetConstantBufferCS(GfxBuffer* buffer, uint slot);
            void SetUnorderedAccessView(GfxBuffer* buffer, uint slot);
            void SetRenderTarget(GfxTexture* renderTarget, GfxTexture* depthStencil = nullptr);
            void SetSamplerState(ID3D11SamplerState* samplerState, uint slot);

            void Copy(GfxBuffer* source, GfxBuffer* dest);
            bool Map(const GfxBufferMapDesc& desc, GfxMappedResource& mappedResource);
            void Unmap(const GfxBufferMapDesc& desc);
            void UpdateSubresource(GfxBuffer* buffer, void* data);

            void Draw(int vertexCount = -1);
            void DrawInstanced(uint instanceCount, int vertexCount = -1);
            void DrawIndexed(int indexCount = -1);
            void DrawIndexedInstanced(uint instanceCount, int indexCount = -1);
            void Dispatch(const uint threadGroupCountX, const uint threadGroupCountY, const uint threadGroupCountZ);

            void ClearColour(GfxTexture& texture);
            void ClearDepth(GfxTexture& texture);
#endif

        private:
            uint          m_vertexCount;
            uint          m_indexCount;
        };
    }
}

#endif // CLOUD_RENDERER_RENDERING_DEVICE_HEADER