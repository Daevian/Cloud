#ifndef CLOUD_RENDERER_CS_SORT_HEADER
#define CLOUD_RENDERER_CS_SORT_HEADER

namespace Cloud
{
    namespace Renderer
    {
        class GfxComputeShader;
        class GfxBuffer;

        struct ElementType
        {
            float f0;
        };

        class CsSorter
        {
        public:
            CsSorter();

            void Init();
            void Uninit();

            void Update();
            void Dispatch();

        private:
            GfxComputeShader* m_shader;
            GfxBuffer* m_structBuffer0;
            GfxBuffer* m_outputBuffer;
            GfxBuffer* m_debugBuffer;

            static const CLuint c_elementCount = 65536;
            Utils::StaticArray<ElementType, c_elementCount> m_buffer0;
            Utils::StaticArray<ElementType, c_elementCount> m_buffer1;

        };
    }
}

#endif // CLOUD_RENDERER_CS_SORT_HEADER