#ifndef CLOUD_RENDERER_GFX_BUFFER_HEADER
#define CLOUD_RENDERER_GFX_BUFFER_HEADER

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
        class GfxBuffer
        {
            friend class GfxBufferFactory;
        public:

            ID3D11Buffer*               GetBuffer() const       { return m_buffer; }
            ID3D11ShaderResourceView*   GetSrv() const          { return m_srv; }
            ID3D11UnorderedAccessView*  GetUav() const          { return m_uav; }

        private:
            GfxBuffer();
            ~GfxBuffer() {};

            ID3D11Buffer*               m_buffer;
            ID3D11ShaderResourceView*   m_srv;
            ID3D11UnorderedAccessView*  m_uav;
            GfxBufferDesc     m_desc;
        };

        class GfxBufferFactory
        {
        public:
            GfxBuffer* Create(const GfxBufferDesc& desc);

            void Destroy(GfxBuffer* buffer);

        private:
            void InitSrv(const GfxBufferDesc& desc, GfxBuffer& buffer);
            void InitUav(const GfxBufferDesc& desc, GfxBuffer& buffer);

        };
    }
}

#endif // CLOUD_RENDERER_GFX_BUFFER_HEADER