#ifndef CLOUD_RENDERER_CS_TEST_HEADER
#define CLOUD_RENDERER_CS_TEST_HEADER

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "GfxBuffer.h"
#include "GfxShaderResourceView.h"

namespace Cloud
{
namespace Renderer
{
    class ShaderEffect;
    class Texture;

    class CSTest
    {
    public:
        struct Structure
        {
            float4 position;
        };

        struct Vertex
        {
            Math::Float4 pos;
            Math::Float2 uv;
        };

        CSTest();
        ~CSTest();

        bool Initialise();
        void Uninitialise();

        void Update();
        void Render();

    private:
        bool InitVBIB();
        Utils::StaticArray<Vertex, 4> m_vertexData;
        Utils::StaticArray<uint32, 6> m_indexData;
        VertexBuffer m_vertexBuffer;
        IndexBuffer m_indexBuffer;

#ifdef USE_DIRECTX12
#else
        Dx::UniquePtr<ID3D11Texture2D> m_textureResource;
        Dx::UniquePtr<ID3D11ShaderResourceView> m_textureSRV;
        Dx::UniquePtr<ID3D11UnorderedAccessView> m_textureUAV;
        Dx::UniquePtr<ID3D11SamplerState> m_samplerState;

        Dx::UniquePtr<ID3D11ComputeShader> m_csShader;
#endif        
        ShaderEffect* m_effect;
    };
}
}

#endif // CLOUD_RENDERER_CS_TEST_HEADER