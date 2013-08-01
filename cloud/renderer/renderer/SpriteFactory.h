#ifndef CLOUD_RENDERER_SPRITE_FACTORY_HEADER
#define CLOUD_RENDERER_SPRITE_FACTORY_HEADER

namespace Cloud
{
    namespace Renderer
    {
        class Sprite;

        class SpriteFactory
        {
        public:
            SpriteFactory();

            CLbool CreateSprite(const std::string& spriteName, Sprite& spriteOutput);

        private:
            CLbool ParseSprite(const std::string& spriteName, Sprite& spriteOutput);

        };
    }
}

#endif // CLOUD_RENDERER_SPRITE_FACTORY_HEADER