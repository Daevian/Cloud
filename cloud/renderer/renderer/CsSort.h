#ifndef CLOUD_RENDERER_CS_SORT_HEADER
#define CLOUD_RENDERER_CS_SORT_HEADER

namespace Cloud
{
    namespace Renderer
    {
        class GfxComputeShader;
        class GfxStructuredBuffer;

        struct ElementType
        {
            int i;
            float f;
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
            GfxStructuredBuffer* m_structBuffer0;
            GfxStructuredBuffer* m_structBuffer1;
            GfxStructuredBuffer* m_outputBuffer;

            static const CLuint c_elementCount = 128;
            Utils::StaticArray<ElementType, c_elementCount> m_buffer0;
            Utils::StaticArray<ElementType, c_elementCount> m_buffer1;

        };
    }
}

#endif // CLOUD_RENDERER_CS_SORT_HEADER