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
                size_t    size;
            };

            Cloud::DebugName                 name;
            uint                      width;
            uint                      height;
            uint                      depth;
#ifdef USE_DIRECTX12
            D3D12_RESOURCE_DIMENSION    dim;
#else
            D3D11_RESOURCE_DIMENSION    dim;
#endif
            uint                      arraySize;
            uint                      mipCount;
            DXGI_FORMAT                 format;
#ifdef USE_DIRECTX12
            D3D12_RESOURCE_FLAGS        flags;
            D3D12_HEAP_TYPE             heapType;
            D3D12_RESOURCE_STATES       initialState;
            D3D12_CLEAR_VALUE           clearValue;
#else
            D3D11_USAGE                 usage;
            uint                      bindFlags;
            uint                      cpuAccessFlags;
            uint                      miscFlags;
#endif
            
            InitialData                 initialData;
            DXGI_SAMPLE_DESC            sampleDesc;
            bool                      isCubeMap;
        };

        class GfxTexture : public GfxResource
        {
            friend class GfxTextureFactory;
        public:
            typedef std::unique_ptr<GfxTexture, Deleter> UniquePtr;

            const GfxTextureDesc&       GetDesc() const         { return m_desc; }
#ifdef USE_DIRECTX12
            ID3D12Resource*               GetResource() const   { return m_resource.Get(); }
            CD3DX12_CPU_DESCRIPTOR_HANDLE GetSrv() const        { return m_srv; }
            CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtv() const        { return m_rtv; }
            CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsv() const        { return m_dsv; }
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
            ComPtr<ID3D12Resource> m_resource;
            CD3DX12_CPU_DESCRIPTOR_HANDLE m_srv = { CD3DX12_DEFAULT() };
            CD3DX12_CPU_DESCRIPTOR_HANDLE m_rtv = { CD3DX12_DEFAULT() };
            CD3DX12_CPU_DESCRIPTOR_HANDLE m_dsv = { CD3DX12_DEFAULT() };
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
            void FillInitialData(   uint width,
                                    uint height,
                                    uint depth,
                                    uint mipCount,
                                    uint arraySize,
                                    DXGI_FORMAT format,
                                    const uint8* imageData,
                                    size_t imageDataSize,
                                    uint& tWidth,
                                    uint& tHeight,
                                    uint& tDepth,
                                    uint& skipMip,
                                    D3D11_SUBRESOURCE_DATA* initData);
#endif
        };
    }
}

#endif // CLOUD_RENDERER_GFX_TEXTURE_HEADER