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
            float f;
        };

        class CsSorter
        {
            struct BitonicSortConstBuffer
            {
                CLuint level;
                CLuint levelMask;
            };

            struct TransposeConstBuffer
            {
                CLuint width;
                CLuint height;
            };

        public:
            CsSorter();

            void Init();
            void Uninit();

            void Update();
            void Dispatch();

        private:
            void UpdateBitonicSortConstBuffer(CLuint level, CLuint levelMask);
            void UpdateTransposeConstBuffer(CLuint width, CLuint height);

            GfxComputeShader* m_bitonicSortShader;
            GfxComputeShader* m_transposeShader;
            GfxBuffer* m_bitonicSortConstBuffer;
            GfxBuffer* m_transposeConstBuffer;
            GfxBuffer* m_structBuffer0;
            GfxBuffer* m_structBuffer1;
            GfxBuffer* m_outputBuffer;
            GfxBuffer* m_debugBuffer;

            static const CLuint c_bitonicBlockSize = 512;
            static const CLuint c_elementCount = 65536;
            Utils::StaticArray<ElementType, c_elementCount> m_buffer0;
        };
    }
}

#endif // CLOUD_RENDERER_CS_SORT_HEADER