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

            CLbool Initialise();
            void Uninitialise();

            CLint   GetInstanceCount()                                   const { return m_instanceCount; }
            CLint   GetInstanceSize()                                    const { return m_instanceSize; }
            void*   GetInstanceData()                                    const { return m_instanceData; }
            ID3D11Buffer* GetBuffer()                                    const { return m_instanceBuffer; }

            void SetInstanceCount(CLint vertexCount)                     { m_instanceCount = vertexCount; }
            void SetInstanceSize(CLint vertexSize)                       { m_instanceSize = vertexSize; }
            void SetInstanceData(void* vertexData)                     { m_instanceData = vertexData; }

            void GpuSetInstanceBuffer();
            void GpuUpdateInstanceBuffer();

        private:
            CLint m_instanceCount;
            CLint m_instanceSize;
            void* m_instanceData;
            ID3D11Buffer* m_instanceBuffer;
        };
    }
}

#endif // CLOUD_RENDERER_GFX_INSTANCE_BUFFER_HEADER