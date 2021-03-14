#ifndef CLOUD_RENDERER_GFX_INSTANCE_BUFFER_HEADER
#define CLOUD_RENDERER_GFX_INSTANCE_BUFFER_HEADER

#include "GfxCommon.h"

namespace Cloud
{
    namespace Renderer
    {
        class GfxInstanceBuffer
        {
        public:
            GfxInstanceBuffer();

            bool Initialise();
            void Uninitialise();

            int   GetInstanceCount()                                   const { return m_instanceCount; }
            int   GetInstanceSize()                                    const { return m_instanceSize; }
            void*   GetInstanceData()                                    const { return m_instanceData; }
#ifdef USE_DIRECTX12
#else
            ID3D11Buffer* GetBuffer()                                    const { return m_instanceBuffer; }
#endif

            void SetInstanceCount(int vertexCount)                     { m_instanceCount = vertexCount; }
            void SetInstanceSize(int vertexSize)                       { m_instanceSize = vertexSize; }
            void SetInstanceData(void* vertexData)                     { m_instanceData = vertexData; }

            void GpuSetInstanceBuffer();
            void GpuUpdateInstanceBuffer();

        private:
            int m_instanceCount;
            int m_instanceSize;
            void* m_instanceData;
#ifdef USE_DIRECTX12
#else
            ID3D11Buffer* m_instanceBuffer;
#endif
        };
    }
}

#endif // CLOUD_RENDERER_GFX_INSTANCE_BUFFER_HEADER