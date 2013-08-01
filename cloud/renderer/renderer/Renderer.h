#ifndef CLOUD_RENDERER_RENDERER_HEADER
#define CLOUD_RENDERER_RENDERER_HEADER

#include "SpriteManager.h"

namespace Cloud
{
    namespace Renderer
    {
        struct ConstantBuffer
        {
            Math::Matrix4 m_projection;
        };

        class Renderer
        {
        public:
            Renderer();

            CLbool Initialise();
            void Shutdown();

            void Render();

            SpriteManager& GetSpriteManager() { return m_spriteManager; }

        private:
            SpriteManager m_spriteManager;
            Math::Matrix4 m_view;
            Math::Matrix4 m_projection;
            ID3D11Buffer* m_constantBuffer;
        };
    }
}

#endif // CLOUD_RENDERER_RENDERER_HEADER