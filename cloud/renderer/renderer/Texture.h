#ifndef CLOUD_RENDERER_TEXTURE_HEADER
#define CLOUD_RENDERER_TEXTURE_HEADER

#include "GfxTexture.h"
#include "DdsHelper.h"

namespace Cloud
{
    namespace Renderer
    {
        class ShaderEffect;

        class Texture
        {
        public:
            Texture(const std::string& texturePath);
            ~Texture();

            bool Load();
            void Unload();

            const std::string& GetPath() const { return m_texturePath; };

            GfxTexture* GetTexture() const              { return m_texture.get(); }
#ifdef USE_DIRECTX12
#else
            ID3D11SamplerState* GetSamplerState() const { return m_samplerState.get(); }
#endif

        private:

            bool LoadResource();
            bool LoadSampler();
            void ReadTextureDataFromFile(const Cloud::String& fileName, std::unique_ptr<uint8_t[]>& ddsData, DdsHeader*& header, DdsHeaderDXT10*& dxt10Header, uint8*& imageData, size_t& imageDataSize);
            bool IsDds(const std::unique_ptr<uint8_t[]>& ddsData);

            GfxTexture::UniquePtr m_texture;
#ifdef USE_DIRECTX12
#else
            Dx::UniquePtr<ID3D11SamplerState> m_samplerState;
#endif
            std::string m_texturePath;

        };
    }
}

#endif // CLOUD_RENDERER_TEXTURE_HEADER