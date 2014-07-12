#ifndef CLOUD_RENDERER_GFX_SHADER_RESOURCE_HEADER
#define CLOUD_RENDERER_GFX_SHADER_RESOURCE_HEADER

namespace Cloud
{
namespace Renderer
{
    class GfxShaderResource
    {
    public:
        GfxShaderResource();

        //ID3D11Buffer* GetBuffer() const   { return m_buffer; }
        //
        //CLuint GetElementSize() const   { return m_elementSize; }
        //CLuint GetElementCount() const  { return m_elementCount; }
        //
        //void SetElementSize(CLuint elementSize)     { m_elementSize = elementSize; }
        //void SetElementCount(CLuint elementCount)   { m_elementCount = elementCount; }
        //
        //CLbool  Init(void* initialData = 0);
        //void    Uninit();

    private:
        //ID3D11Buffer* m_buffer;

        //CLuint m_elementSize;
        //CLuint m_elementCount;
    };
}
}

#endif // CLOUD_RENDERER_GFX_SHADER_RESOURCE_HEADER