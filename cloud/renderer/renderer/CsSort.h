#ifndef CLOUD_RENDERER_CS_SORT_HEADER
#define CLOUD_RENDERER_CS_SORT_HEADER

#include "GfxBuffer.h"
#include "GfxShader.h"

namespace Cloud
{
    namespace Renderer
    {
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
            ~CsSorter();

            void Init();
            void Uninit();

            void Update();
            void Dispatch();

        private:
            void UpdateBitonicSortConstBuffer(CLuint level, CLuint levelMask);
            void UpdateTransposeConstBuffer(CLuint width, CLuint height);

            GfxComputeShader::UniquePtr m_bitonicSortShader;
            GfxComputeShader::UniquePtr m_transposeShader;
            GfxBuffer::UniquePtr m_bitonicSortConstBuffer;
            GfxBuffer::UniquePtr m_transposeConstBuffer;
            GfxBuffer::UniquePtr m_structBuffer0;
            GfxBuffer::UniquePtr m_structBuffer1;
            GfxBuffer::UniquePtr m_outputBuffer;
            GfxBuffer::UniquePtr m_debugBuffer;

            static const CLuint c_bitonicBlockSize = 512;
            static const CLuint c_elementCount = 65536;
            std::array<ElementType, c_elementCount> m_buffer0;
        };
    }
}

#endif // CLOUD_RENDERER_CS_SORT_HEADER