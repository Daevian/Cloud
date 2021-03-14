#ifndef CLOUD_RENDERER_VERTEXBUFFER_HEADER
#define CLOUD_RENDERER_VERTEXBUFFER_HEADER

#include "GfxCommon.h"

namespace Cloud
{
    namespace Renderer
    {
        class VertexBuffer
        {
        public:
            VertexBuffer();
            VertexBuffer(const VertexBuffer& vertexBuffer) = delete;
            VertexBuffer(VertexBuffer&& vertexBuffer);


            bool Initialise();
            void Uninitialise();

            int   GetVertexCount()                const { return m_vertexCount; }
            int   GetVertexSize()                 const { return m_vertexSize; }
            const void*   GetVertexData()           const { return m_vertexData; }
#ifdef USE_DIRECTX12
            const D3D12_VERTEX_BUFFER_VIEW& GetView() const { return m_view; }
#else
            ID3D11Buffer* GetBuffer()               const { return m_vertexBuffer.get(); }
            GfxPrimitiveTopology GetTopology()      const { return m_topology; }
            void SetTopology(GfxPrimitiveTopology topology) { m_topology = topology; }
#endif

            void SetVertexCount(int vertexCount)              { m_vertexCount = vertexCount; }
            void SetVertexSize(int vertexSize)                { m_vertexSize = vertexSize; }
            void SetVertexData(const void* vertexData)          { m_vertexData = vertexData; }
            

            void GPUUpdateVertexBuffer();
            void GPUUpdateVertexBuffer(void* data, size_t size, size_t offset = 0);

        private:
            int m_vertexCount;
            int m_vertexSize;
            const void* m_vertexData;
#ifdef USE_DIRECTX12
            ComPtr<ID3D12Resource> m_buffer;
            D3D12_VERTEX_BUFFER_VIEW m_view;
            void* m_bufferData = nullptr;
#else
            GfxPrimitiveTopology m_topology;
            Dx::UniquePtr<ID3D11Buffer> m_vertexBuffer;
#endif
        };
    }
}

#endif // CLOUD_RENDERER_VERTEXBUFFER_HEADER