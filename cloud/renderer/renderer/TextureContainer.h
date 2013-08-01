#ifndef CLOUD_RENDERER_TEXTURECONTAINER_HEADER
#define CLOUD_RENDERER_TEXTURECONTAINER_HEADER

#include "../../core_libraries/utilities/DynamicArray.h"

namespace Cloud
{
    namespace Renderer
    {
        class Texture;

        class TextureContainer
        {
        public:
            TextureContainer();
            ~TextureContainer();

            Texture* GetTexture(const std::string& texturePath);

        private:
            Texture* LoadTexture(const std::string& texturePath);

            Utils::DynamicArray<Texture*> m_textures;

        };
    }
}

#endif // CLOUD_RENDERER_TEXTURECONTAINER_HEADER