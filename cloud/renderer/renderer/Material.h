#pragma once

#include "ResourceContainer.h"

namespace Cloud
{
    namespace Renderer
    {
        class ShaderEffect;
        class GfxConstantBuffer;

        struct MaterialData
        {
            Math::Float4 f0 = Math::Float4(1.0f, 0.71f, 0.29f, 1.0f); // gold
            CLfloat roughness = 0.15f;
        };

        class Material : public Resource
        {
        public:
            Material(const ResourceId& id);

            const ShaderEffect& GetPso() const              { return *m_pso; }
            const GfxConstantBuffer& GetConstBuffer() const { return *m_materialConstBuffer; }

            void PopulateDebugMenu();

        private:
            std::shared_ptr<ShaderEffect> m_pso;
            std::unique_ptr<GfxConstantBuffer> m_materialConstBuffer;
            MaterialData m_materialData;
        };
    }
}