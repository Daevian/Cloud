#ifndef CLOUD_RENDERER_GFX_BUFFER_HEADER
#define CLOUD_RENDERER_GFX_BUFFER_HEADER

#include "GfxResource.h"

namespace Cloud
{
    namespace Renderer
    {
        struct GfxBufferDesc
        {
            Cloud::DebugName name;
            uint      elementSize;
            uint      elementCount;
#ifdef USE_DIRECTX12
#else
            D3D11_USAGE usage;
#endif
            uint      bindFlags;
            uint      cpuAccessFlags;
            uint      miscFlags;
            void*       initialData;
        };

        // should be more than just a structured buffer
        class GfxBuffer : public GfxResource
        {
            friend class GfxBufferFactory;
        public:
            typedef std::unique_ptr<GfxBuffer, Deleter> UniquePtr;

            const GfxBufferDesc&        GetDesc() const         { return m_desc; }
#ifdef USE_DIRECTX12
#else
            ID3D11Buffer*               GetBuffer() const       { return m_buffer.get(); }
            ID3D11ShaderResourceView*   GetSrv() const          { return m_srv.get(); }
            ID3D11UnorderedAccessView*  GetUav() const          { return m_uav.get(); }
#endif

        private:
            GfxBuffer();
            virtual ~GfxBuffer() override;

            static UniquePtr MakeUnique()   { return GfxBuffer::UniquePtr(new GfxBuffer(), GfxResource::Deleter()); }

#ifdef USE_DIRECTX12
#else
            Dx::UniquePtr<ID3D11Buffer>                 m_buffer;
            Dx::UniquePtr<ID3D11ShaderResourceView>     m_srv;
            Dx::UniquePtr<ID3D11UnorderedAccessView>    m_uav;
#endif
            GfxBufferDesc                               m_desc;
        };

        class GfxBufferFactory
        {
        public:
            GfxBuffer::UniquePtr Create(const GfxBufferDesc& desc);

        private:
            void InitSrv(const GfxBufferDesc& desc, GfxBuffer& buffer);
            void InitUav(const GfxBufferDesc& desc, GfxBuffer& buffer);

            bool VerifySetup(const GfxBufferDesc& desc);

        };
    }
}

#endif // CLOUD_RENDERER_GFX_BUFFER_HEADER