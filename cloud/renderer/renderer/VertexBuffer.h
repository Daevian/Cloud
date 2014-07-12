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
            VertexBuffer(const VertexBuffer& vertexBuffer);

            CLbool Initialise();
            void Uninitialise();

            CLint   GetVertexCount()                const { return m_vertexCount; }
            CLint   GetVertexSize()                 const { return m_vertexSize; }
            void* GetVertexData()                 const { return m_vertexData; }
            ID3D11Buffer* GetBuffer()               const { return m_vertexBuffer; }
            GfxPrimitiveTopology GetTopology()      const { return m_topology; }

            void SetVertexCount(CLint vertexCount)              { m_vertexCount = vertexCount; }
            void SetVertexSize(CLint vertexSize)                { m_vertexSize = vertexSize; }
            void SetVertexData(void* vertexData)              { m_vertexData = vertexData; }
            void SetTopology(GfxPrimitiveTopology topology)     { m_topology = topology; }

            void GPUUpdateVertexBuffer();

        private:
            CLint m_vertexCount;
            CLint m_vertexSize;
            void* m_vertexData;
            GfxPrimitiveTopology m_topology;
            ID3D11Buffer* m_vertexBuffer;
        };
    }
}

#endif // CLOUD_RENDERER_VERTEXBUFFER_HEADER