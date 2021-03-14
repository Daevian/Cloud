#ifndef CLOUD_RENDERER_SPRITE_HEADER
#define CLOUD_RENDERER_SPRITE_HEADER

#include "../../core_libraries/utilities/Float4.h"
#include "../../core_libraries/utilities/Float2.h"

#include "VertexBuffer.h"

namespace Cloud
{
    namespace Renderer
    {
        struct SpriteVertex
        {
            Math::Float2 pos;
            Math::Float2 scale;
        };

        class ShaderEffect;
        class Texture;

        class Sprite
        {
        public:
            Sprite();
            Sprite(const Sprite& sprite) = delete;
            Sprite(Sprite&& sprite);

            bool Init(Texture* texture);
            void Unload();

            void Render();

            void Translate(const Math::Float2& translation) { m_vertex.pos += translation; }

            void SetPosition(const Math::Float2& position)      { m_vertex.pos = position; }
            void SetScale(const Math::Float2& scale)            { m_vertex.scale = scale; }
            void SetVisible(bool visible)                     { m_visible = visible; }

            bool GetVisible() const                           { return m_visible; }

        private:
            VertexBuffer m_vertexBuffer;

            ShaderEffect* m_effect;
            Texture* m_texture;

            SpriteVertex m_vertex;

            bool m_visible;
        };
    }
}

#endif