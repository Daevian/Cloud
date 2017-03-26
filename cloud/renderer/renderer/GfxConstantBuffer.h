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
        CLbool Initialise(CLint dataSize, const void* initialData, CLuint versions = 1);
        CLbool Initialise(CLuint versions = 1);
        void Uninitialise();

        void SetData(const void* data, CLint dataSize);

        void GPUUpdateConstantBuffer();

        D3D12_GPU_VIRTUAL_ADDRESS UpdateConstantBuffer(const void* data);
        D3D12_GPU_VIRTUAL_ADDRESS GetCurrentVersionGpuAddress() const;

        void Reset();

#ifdef USE_DIRECTX12
        ID3D12Resource* GetBuffer() const { return m_constantBuffer.Get(); }
#else
        ID3D11Buffer* GetBuffer()   { return m_constantBuffer; }
#endif

    private:
        const void*   m_data = nullptr;
        CLint   m_dataSize = 0;
        CLuint  m_versions = 0;
        CLint  m_currentVersion = -1;
#ifdef USE_DIRECTX12
        ComPtr<ID3D12Resource> m_constantBuffer;
        CD3DX12_CPU_DESCRIPTOR_HANDLE m_handle;
        CLbyte* m_bufferData;
#else
        ID3D11Buffer* m_constantBuffer;
#endif

        static const CLuint c_cbufferAlignment = 256;
    };
}
}

#endif // CLOUD_RENDERER_GFX_CONSTANT_BUFFER_HEADER