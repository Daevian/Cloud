#ifndef CLOUD_RENDERER_GFX_BUFFER_HEADER
#define CLOUD_RENDERER_GFX_BUFFER_HEADER

namespace Cloud
{
    namespace Renderer
    {
        struct GfxStructuredBufferDesc
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

        class GfxStructuredBuffer
        {
            friend class GfxBufferFactory;
        public:

        private:
            GfxStructuredBuffer();
            ~GfxStructuredBuffer() {};

            ID3D11Buffer*               m_buffer;
            ID3D11ShaderResourceView*   m_srv;
            ID3D11UnorderedAccessView*  m_uav;
            GfxStructuredBufferDesc     m_desc;
        };

        class GfxBufferFactory
        {
        public:
            GfxStructuredBuffer* Create(const GfxStructuredBufferDesc& desc);

            void Destroy(GfxStructuredBuffer* buffer);

        private:
            void InitSrv(const GfxStructuredBufferDesc& desc, GfxStructuredBuffer& buffer);
            void InitUav(const GfxStructuredBufferDesc& desc, GfxStructuredBuffer& buffer);

        };
    }
}

#endif // CLOUD_RENDERER_GFX_BUFFER_HEADER