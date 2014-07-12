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
            IndexBuffer(const IndexBuffer& vertexBuffer);

            CLbool Initialise();
            void Uninitialise();

            CLint GetIndexCount() const                     { return m_indexCount; }
            void* GetIndexData()  const                     { return m_indexData; }
            ID3D11Buffer* GetBuffer() const                 { return m_indexBuffer; }

            void SetIndexCount(CLint indexCount)            { m_indexCount = indexCount; }
            void SetIndexData(void* indexData)              { m_indexData = indexData; }

            void GPUUpdateIndexBuffer();

        private:
            CLint m_indexCount;
            CLint m_indexSize;
            void* m_indexData;
            ID3D11Buffer* m_indexBuffer;
        };
    }
}

#endif // CLOUD_RENDERER_INDEXBUFFER_HEADER