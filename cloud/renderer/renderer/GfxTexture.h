#ifndef CLOUD_RENDERER_GFX_TEXTURE_HEADER
#define CLOUD_RENDERER_GFX_TEXTURE_HEADER

#include "GfxResource.h"

namespace Cloud
{
    namespace Renderer
    {
        struct GfxTextureDesc
        {
            struct InitialData
            {
                void*       data;
                CLsize_t    size;
            };

            ClDebugName                 name;
            CLuint                      width;
            CLuint                      height;
            CLuint                      depth;
#ifdef USE_DIRECTX12
#else
            D3D11_RESOURCE_DIMENSION    dim;
#endif
            CLuint                      arraySize;
            CLuint                      mipCount;
#ifdef USE_DIRECTX12
#else
            DXGI_FORMAT                 format;
            D3D11_USAGE                 usage;
#endif
            CLuint                      bindFlags;
            CLuint                      cpuAccessFlags;
            CLuint                      miscFlags;
            InitialData                 initialData;
#ifdef USE_DIRECTX12
#else
            DXGI_SAMPLE_DESC            sampleDesc;
#endif
            CLbool                      isCubeMap;
        };

        class GfxTexture : public GfxResource
        {
            friend class GfxTextureFactory;
        public:
            typedef std::unique_ptr<GfxTexture, Deleter> UniquePtr;

            const GfxTextureDesc&       GetDesc() const         { return m_desc; }
#ifdef USE_DIRECTX12
#else
            ID3D11ShaderResourceView*   GetSrv() const          { return m_srv.get(); }
            ID3D11RenderTargetView*     GetRtv() const          { return m_rtv.get(); }
            ID3D11DepthStencilView*     GetDsv() const          { return m_dsv.get(); }
#endif

        private:
            GfxTexture();
            virtual ~GfxTexture() override;

            static UniquePtr MakeUnique() { return GfxTexture::UniquePtr(new GfxTexture(), GfxResource::Deleter()); }

#ifdef USE_DIRECTX12
#else
            Dx::UniquePtr<ID3D11Texture2D>            m_texture;
            Dx::UniquePtr<ID3D11ShaderResourceView>   m_srv;
            Dx::UniquePtr<ID3D11RenderTargetView>     m_rtv;
            Dx::UniquePtr<ID3D11DepthStencilView>     m_dsv;
#endif
            GfxTextureDesc              m_desc;
        };

        class GfxTextureFactory
        {
        public:
            Cloud::Renderer::GfxTexture::UniquePtr Create(const GfxTextureDesc& desc);
            Cloud::Renderer::GfxTexture::UniquePtr CreateFromBackbuffer();

        private:
            void Init2d(const GfxTextureDesc& desc, GfxTexture& texture);
            void InitSrv(const GfxTextureDesc& desc, GfxTexture& texture);
            void InitRtv(const GfxTextureDesc& desc, GfxTexture& texture);
            void InitDsv(const GfxTextureDesc& desc, GfxTexture& texture);

#ifdef USE_DIRECTX12
#else
            void FillInitialData(   CLuint width,
                                    CLuint height,
                                    CLuint depth,
                                    CLuint mipCount,
                                    CLuint arraySize,
                                    DXGI_FORMAT format,
                                    const CLuint8* imageData,
                                    CLsize_t imageDataSize,
                                    CLuint& tWidth,
                                    CLuint& tHeight,
                                    CLuint& tDepth,
                                    CLuint& skipMip,
                                    D3D11_SUBRESOURCE_DATA* initData);
#endif
        };
    }
}

#endif // CLOUD_RENDERER_GFX_TEXTURE_HEADER