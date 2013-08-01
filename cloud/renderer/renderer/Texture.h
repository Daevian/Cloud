#ifndef CLOUD_RENDERER_TEXTURE_HEADER
#define CLOUD_RENDERER_TEXTURE_HEADER

namespace Cloud
{
    namespace Renderer
    {
        class ShaderEffect;

        class Texture
        {
        public:
            Texture(const std::string& texturePath);

            CLbool Load();
            void Unload();

            const std::string& GetPath() const { return m_texturePath; };

            void GPUSetShaderResource();
            void GPUSetSamplerState();

        private:

            CLbool LoadResource();
            CLbool LoadSampler();

            ID3D11ShaderResourceView* m_textureRV;
            ID3D11SamplerState* m_samplerState;
            std::string m_texturePath;

        };
    }
}

#endif // CLOUD_RENDERER_TEXTURE_HEADER