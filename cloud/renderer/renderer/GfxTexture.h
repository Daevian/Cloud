#ifndef CLOUD_RENDERER_GFX_TEXTURE_HEADER
#define CLOUD_RENDERER_GFX_TEXTURE_HEADER

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
            D3D11_RESOURCE_DIMENSION    dim;
            CLuint                      arraySize;
            CLuint                      mipCount;
            DXGI_FORMAT                 format;
            D3D11_USAGE                 usage;
            CLuint                      bindFlags;
            CLuint                      cpuAccessFlags;
            CLuint                      miscFlags;
            InitialData                 initialData;
            DXGI_SAMPLE_DESC            sampleDesc;
            CLbool                      isCubeMap;
        };

        class GfxTexture
        {
            friend class GfxTextureFactory;
        public:
            const GfxTextureDesc&       GetDesc() const         { return m_desc; }
            ID3D11ShaderResourceView*   GetSrv() const          { return m_srv; }
            ID3D11RenderTargetView*     GetRtv() const          { return m_rtv; }
            ID3D11DepthStencilView*     GetDsv() const          { return m_dsv; }

        private:
            GfxTexture();
            ~GfxTexture() {};

            ID3D11Texture2D*            m_texture;
            ID3D11ShaderResourceView*   m_srv;
            ID3D11RenderTargetView*     m_rtv;
            ID3D11DepthStencilView*     m_dsv;
            GfxTextureDesc              m_desc;
        };

        class GfxTextureFactory
        {
        public:
            GfxTexture* Create(const GfxTextureDesc& desc);
            void Destroy(GfxTexture* texture);

            GfxTexture* CreateFromBackbuffer();

        private:
            void Init2d(const GfxTextureDesc& desc, GfxTexture& texture);
            void InitSrv(const GfxTextureDesc& desc, GfxTexture& texture);
            void InitRtv(const GfxTextureDesc& desc, GfxTexture& texture);
            void InitDsv(const GfxTextureDesc& desc, GfxTexture& texture);

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
        };
    }
}

#endif // CLOUD_RENDERER_GFX_TEXTURE_HEADER