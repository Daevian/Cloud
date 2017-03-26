#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "GfxConstantBuffer.h"

namespace Cloud
{
    namespace Renderer
    {
        class Material;

        class Mesh
        {
        public:
            Mesh();

            const VertexBuffer& GetVertexBuffer() const { return *m_vb; }
            const IndexBuffer& GetIndexBuffer() const { return *m_ib; }

        private:
            std::unique_ptr<VertexBuffer> m_vb;
            std::unique_ptr<IndexBuffer> m_ib;
        };

        class ModelInstance
        {
        public:
            ModelInstance();

            const ClMatrix4& GetTransform() const   { return m_transform; }
            ClMatrix4& GetTransformMutable()        { return m_transform; }
            CLbool GetVisible() const               { return m_visible;}

            const Material& GetMaterial() const     { return *m_material; }
            const Mesh& GetMesh() const             { return *m_mesh; }


            void PopulateDebugMenu();
            
        private:
            std::shared_ptr<Material> m_material;
            std::shared_ptr<Mesh>     m_mesh;
            
            ClMatrix4       m_transform = ClMatrix4::Identity();
            CLbool          m_visible = true;
        };

        class ModelRenderer
        {
        public:
            ModelRenderer();
            ~ModelRenderer();
            static ModelRenderer& Instance() { return *s_instance; }

            void Initialise();

            void Render(ID3D12GraphicsCommandList* commandList, const ModelInstance& modelInstance);

            void PopulateDebugMenu();

        private:
            static ModelRenderer* s_instance;
        };
    }
}