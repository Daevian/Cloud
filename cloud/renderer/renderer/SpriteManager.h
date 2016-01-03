#ifndef CLOUD_RENDERER_SPRITE_MANAGER_HEADER
#define CLOUD_RENDERER_SPRITE_MANAGER_HEADER

#include "Sprite.h"
#include "SpriteFactory.h"

namespace Cloud
{
    namespace Renderer
    {
        class SpriteManager
        {
        public:
            SpriteManager();

            void Unload();

            Sprite* CreateSprite(const std::string& spriteName);

            void Render();

        private:
            SpriteFactory m_spriteFactory;
            std::vector<Sprite> m_sprites;

        };
    }
}

#endif // CLOUD_RENDERER_SPRITE_MANAGER_HEADER