#ifndef CLOUD_RENDERER_GFX_CONSTANT_BUFFER_HEADER
#define CLOUD_RENDERER_GFX_CONSTANT_BUFFER_HEADER

namespace Cloud
{
namespace Renderer
{
    class GfxConstantBuffer
    {
    public:
        GfxConstantBuffer();
        CLbool Initialise();
        void Uninitialise();

        void SetData(void* data, CLint dataSize);

        void GPUUpdateConstantBuffer();

        ID3D11Buffer* getBuffer()   { return m_constantBuffer; }

    private:
        void*         m_data;
        CLint         m_dataSize;
        ID3D11Buffer* m_constantBuffer;
    };
}
}

#endif // CLOUD_RENDERER_GFX_CONSTANT_BUFFER_HEADER