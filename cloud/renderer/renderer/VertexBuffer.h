#ifndef CLOUD_RENDERER_VERTEXBUFFER_HEADER
#define CLOUD_RENDERER_VERTEXBUFFER_HEADER

namespace Cloud
{
    namespace Renderer
    {
        class VertexBuffer
        {
        public:
            VertexBuffer();
            VertexBuffer(const VertexBuffer& vertexBuffer);

            CLbool Init();
            void Unload();

            CLint   GetVertexCount()    const { return m_vertexCount; }
            CLint   GetVertexSize()     const { return m_vertexSize; }
            CLchar* GetVertexData()     const { return m_vertexData; }

            void SetVertexCount(CLint vertexCount)              { m_vertexCount = vertexCount; }
            void SetVertexSize(CLint vertexSize)                { m_vertexSize = vertexSize; }
            void SetVertexData(CLchar* vertexData)              { m_vertexData = vertexData; }
            void SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology) { m_topology = topology; }

            void GPUSetVertexBuffer();
            void GPUUpdateVertexBuffer();
            void GPUSetTopology();

        private:
            CLint m_vertexCount;                    //4
            CLint m_vertexSize;                     //4
            CLchar* m_vertexData;                   //4
            D3D11_PRIMITIVE_TOPOLOGY m_topology;    //4
            ID3D11Buffer* m_vertexBuffer;           //4
        }; //20
    }
}

#endif // CLOUD_RENDERER_VERTEXBUFFER_HEADER