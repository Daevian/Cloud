#ifndef CLOUD_RENDERER_INDEXBUFFER_HEADER
#define CLOUD_RENDERER_INDEXBUFFER_HEADER

namespace Cloud
{
    namespace Renderer
    {
        class IndexBuffer
        {
        public:
            IndexBuffer();
            IndexBuffer(const IndexBuffer& vertexBuffer) = delete;
            IndexBuffer(IndexBuffer&& vertexBuffer);

            CLbool Initialise();
            void Uninitialise();

            CLint GetIndexCount() const                     { return m_indexCount; }
            void* GetIndexData()  const                     { return m_indexData; }
#ifdef USE_DIRECTX12
            const D3D12_INDEX_BUFFER_VIEW& GetView() const  { return m_view; }
#else
            ID3D11Buffer* GetBuffer() const                 { return m_indexBuffer.get(); }
#endif

            void SetIndexCount(CLint indexCount)            { m_indexCount = indexCount; }
            void SetIndexData(void* indexData)              { m_indexData = indexData; }

            void GPUUpdateIndexBuffer();

        private:
            CLint m_indexCount;
            CLint m_indexSize;
            void* m_indexData;
#ifdef USE_DIRECTX12
            ComPtr<ID3D12Resource> m_buffer;
            D3D12_INDEX_BUFFER_VIEW m_view;
#else
            Dx::UniquePtr<ID3D11Buffer> m_indexBuffer;
#endif
        };
    }
}

#endif // CLOUD_RENDERER_INDEXBUFFER_HEADER