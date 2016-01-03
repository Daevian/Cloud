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
            ClFloat4 position;
        };

        struct Vertex
        {
            Math::Float4 pos;
            Math::Float2 uv;
        };

        CSTest();
        ~CSTest();

        CLbool Initialise();
        void Uninitialise();

        void Update();
        void Render();

    private:
        CLbool InitVBIB();
        Utils::StaticArray<Vertex, 4> m_vertexData;
        Utils::StaticArray<CLuint32, 6> m_indexData;
        VertexBuffer m_vertexBuffer;
        IndexBuffer m_indexBuffer;

        Dx::UniquePtr<ID3D11Texture2D> m_textureResource;
        Dx::UniquePtr<ID3D11ShaderResourceView> m_textureSRV;
        Dx::UniquePtr<ID3D11UnorderedAccessView> m_textureUAV;
        Dx::UniquePtr<ID3D11SamplerState> m_samplerState;
        
        ShaderEffect* m_effect;

        Dx::UniquePtr<ID3D11ComputeShader> m_csShader;
    };
}
}

#endif // CLOUD_RENDERER_CS_TEST_HEADER