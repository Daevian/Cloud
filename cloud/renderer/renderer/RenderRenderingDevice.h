#ifndef CLOUD_RENDERER_RENDERING_DEVICE_HEADER
#define CLOUD_RENDERER_RENDERING_DEVICE_HEADER

namespace Cloud
{
    namespace Renderer
    {
        class ShaderEffect;
        class VertexBuffer;
        class Texture;

        class RenderingDevice
        {
        public:
            RenderingDevice();

            CLbool Init();

            void SetEffect(ShaderEffect* effect);
            void SetVertexBuffer(VertexBuffer* vertexBuffer);
            void SetTexture(Texture* texture);

            CLbool Render();

        private:
            ShaderEffect* m_effect;
            VertexBuffer* m_vertexBuffer;
            Texture* m_texture;
        };
    }
}

#endif // CLOUD_RENDERER_RENDERING_DEVICE_HEADER