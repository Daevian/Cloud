#pragma once

namespace Cloud
{
    namespace Renderer
    {
        class VertexBuffer;
        class IndexBuffer;

        class Mesh
        {
        public:
            struct Vertex
            {
                ClFloat4 pos;
                ClFloat3 norm;
            };

            Mesh(const std::vector<Vertex>& vertices, const std::vector<CLuint32>& indices);

            const VertexBuffer& GetVertexBuffer() const { return *m_vb; }
            const IndexBuffer& GetIndexBuffer() const { return *m_ib; }

            static std::shared_ptr<Mesh> CreateSphere(CLuint latBands, CLuint longBands);
            static std::shared_ptr<Mesh> CreatePlane();

        private:
            std::unique_ptr<VertexBuffer> m_vb;
            std::unique_ptr<IndexBuffer> m_ib;
        };
    }
}