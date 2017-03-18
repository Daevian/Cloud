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

#ifdef USE_DIRECTX12
#else
        ID3D11Buffer* getBuffer()   { return m_constantBuffer; }
#endif

    private:
        void*         m_data;
        CLint         m_dataSize;
#ifdef USE_DIRECTX12
        ComPtr<ID3D12Resource> m_constantBuffer;
        CD3DX12_CPU_DESCRIPTOR_HANDLE m_handle;
        CLbyte* m_bufferData;
#else
        ID3D11Buffer* m_constantBuffer;
#endif
    };
}
}

#endif // CLOUD_RENDERER_GFX_CONSTANT_BUFFER_HEADER