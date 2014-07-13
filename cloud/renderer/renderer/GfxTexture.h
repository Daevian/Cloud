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
            CLbool                      isCubeMap;
        };

        class GfxTexture
        {
            friend class RenderCore;
        public:
            const GfxTextureDesc& GetDesc() const           { return m_desc; }
            ID3D11ShaderResourceView* GetSrv() const        { return m_srv; }

        private:
            GfxTexture();
            ~GfxTexture();

            void Init(const GfxTextureDesc& desc);
            void Init2d();
            void InitSrv();
            void Destroy();


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

            ID3D11Texture2D*            m_texture;
            ID3D11ShaderResourceView*   m_srv;
            GfxTextureDesc              m_desc;
        };
    }
}

#endif // CLOUD_RENDERER_GFX_TEXTURE_HEADER