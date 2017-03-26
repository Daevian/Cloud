#pragma once

namespace Cloud
{
    namespace Renderer
    {
        class ModelInstance;
        class ModelRenderer;
        class LightCollection;

        class Scene
        {
        public:
            Scene(ModelRenderer& modelRenderer, const LightCollection* lightCollection = nullptr);

            void AddModelInstance(const ModelInstance& modelInstance);

            void RecordCommandList(ID3D12GraphicsCommandList* commandList);

            void PopulateDebugMenu();

        private:
            ModelRenderer& m_modelRenderer;
            std::vector<const ModelInstance*> m_instances;
            const LightCollection* m_lightCollection;
        };
    }
}