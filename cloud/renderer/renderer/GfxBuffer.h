#ifndef CLOUD_RENDERER_GFX_BUFFER_HEADER
#define CLOUD_RENDERER_GFX_BUFFER_HEADER

#include "GfxResource.h"

namespace Cloud
{
    namespace Renderer
    {
        struct GfxBufferDesc
        {
            ClDebugName name;
            CLuint      elementSize;
            CLuint      elementCount;
            D3D11_USAGE usage;
            CLuint      bindFlags;
            CLuint      cpuAccessFlags;
            CLuint      miscFlags;
            void*       initialData;
        };

        // should be more than just a structured buffer
        class GfxBuffer : public GfxResource
        {
            friend class GfxBufferFactory;
        public:
            typedef std::unique_ptr<GfxBuffer, Deleter> UniquePtr;

            const GfxBufferDesc&        GetDesc() const         { return m_desc; }
            ID3D11Buffer*               GetBuffer() const       { return m_buffer; }
            ID3D11ShaderResourceView*   GetSrv() const          { return m_srv; }
            ID3D11UnorderedAccessView*  GetUav() const          { return m_uav; }

        private:
            GfxBuffer();
            virtual ~GfxBuffer() override;

            static UniquePtr MakeUnique()   { return GfxBuffer::UniquePtr(new GfxBuffer(), GfxResource::Deleter()); }

            ID3D11Buffer*               m_buffer;
            ID3D11ShaderResourceView*   m_srv;
            ID3D11UnorderedAccessView*  m_uav;
            GfxBufferDesc     m_desc;
        };

        class GfxBufferFactory
        {
        public:
            GfxBuffer::UniquePtr Create(const GfxBufferDesc& desc);

        private:
            void InitSrv(const GfxBufferDesc& desc, GfxBuffer& buffer);
            void InitUav(const GfxBufferDesc& desc, GfxBuffer& buffer);

            CLbool VerifySetup(const GfxBufferDesc& desc);

        };
    }
}

#endif // CLOUD_RENDERER_GFX_BUFFER_HEADER