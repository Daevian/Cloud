#ifndef CLOUD_RENDERER_INDEXBUFFER_HEADER
#define CLOUD_RENDERER_INDEXBUFFER_HEADER

namespace Cloud
{
    namespace Renderer
    {
        class IndexBuffer
        {
        public:
            IndexBuffer(int indexSize = sizeof(uint32));
            IndexBuffer(const IndexBuffer& vertexBuffer) = delete;
            IndexBuffer(IndexBuffer&& vertexBuffer);

            bool Initialise();
            void Uninitialise();

            int GetIndexCount() const                     { return m_indexCount; }
            const void* GetIndexData()  const               { return m_indexData; }
#ifdef USE_DIRECTX12
            const D3D12_INDEX_BUFFER_VIEW& GetView() const  { return m_view; }
#else
            ID3D11Buffer* GetBuffer() const                 { return m_indexBuffer.get(); }
#endif

            void SetIndexCount(int indexCount)            { m_indexCount = indexCount; }
            void SetIndexData(const void* indexData)        { m_indexData = indexData; }

            void GPUUpdateIndexBuffer();
            void GPUUpdateIndexBuffer(void* data, size_t size, size_t offset = 0);

        private:
            int m_indexCount;
            int m_indexSize;
            const void* m_indexData;
#ifdef USE_DIRECTX12
            ComPtr<ID3D12Resource> m_buffer;
            D3D12_INDEX_BUFFER_VIEW m_view;
            void* m_bufferData = nullptr;
#else
            Dx::UniquePtr<ID3D11Buffer> m_indexBuffer;
#endif
        };
    }
}

#endif // CLOUD_RENDERER_INDEXBUFFER_HEADER