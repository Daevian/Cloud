#pragma once

namespace Cloud
{
    namespace Renderer
    {
        class ModelInstance;

        class ModelLoader
        {
        public:


            std::unique_ptr<ModelInstance> LoadModel(const t_char* modelPath);


        private:
            Assimp::Importer m_assetImporter;
        };
    }
}