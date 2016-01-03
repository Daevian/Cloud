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

            CLbool Load();
            void Unload();

            const std::string& GetPath() const { return m_texturePath; };

            GfxTexture* GetTexture() const              { return m_texture.get(); }
            ID3D11SamplerState* GetSamplerState() const { return m_samplerState.get(); }

        private:

            CLbool LoadResource();
            CLbool LoadSampler();
            void ReadTextureDataFromFile(const ClString& fileName, std::unique_ptr<uint8_t[]>& ddsData, DdsHeader*& header, DdsHeaderDXT10*& dxt10Header, CLuint8*& imageData, CLsize_t& imageDataSize);
            CLbool IsDds(const std::unique_ptr<uint8_t[]>& ddsData);

            GfxTexture::UniquePtr m_texture;
            Dx::UniquePtr<ID3D11SamplerState> m_samplerState;
            std::string m_texturePath;

        };
    }
}

#endif // CLOUD_RENDERER_TEXTURE_HEADER