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
        bool Initialise(int dataSize, const void* initialData, uint versions = 1);
        bool Initialise(uint versions = 1);
        void Uninitialise();

        void SetData(const void* data, int dataSize);

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
        int   m_dataSize = 0;
        uint  m_versions = 0;
        int  m_currentVersion = -1;
#ifdef USE_DIRECTX12
        ComPtr<ID3D12Resource> m_constantBuffer;
        CD3DX12_CPU_DESCRIPTOR_HANDLE m_handle;
        byte* m_bufferData;
#else
        ID3D11Buffer* m_constantBuffer;
#endif

        static const uint c_cbufferAlignment = 256;
    };
}
}

#endif // CLOUD_RENDERER_GFX_CONSTANT_BUFFER_HEADER