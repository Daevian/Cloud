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


            CLbool Initialise();
            void Uninitialise();

            CLint   GetVertexCount()                const { return m_vertexCount; }
            CLint   GetVertexSize()                 const { return m_vertexSize; }
            void*   GetVertexData()                 const { return m_vertexData; }
#ifdef USE_DIRECTX12
            D3D12_VERTEX_BUFFER_VIEW& GetView()  { return m_view; }
#else
            ID3D11Buffer* GetBuffer()               const { return m_vertexBuffer.get(); }
            GfxPrimitiveTopology GetTopology()      const { return m_topology; }
            void SetTopology(GfxPrimitiveTopology topology) { m_topology = topology; }
#endif

            void SetVertexCount(CLint vertexCount)              { m_vertexCount = vertexCount; }
            void SetVertexSize(CLint vertexSize)                { m_vertexSize = vertexSize; }
            void SetVertexData(void* vertexData)              { m_vertexData = vertexData; }
            

            void GPUUpdateVertexBuffer();

        private:
            CLint m_vertexCount;
            CLint m_vertexSize;
            void* m_vertexData;
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