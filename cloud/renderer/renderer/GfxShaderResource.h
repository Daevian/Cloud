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
        //uint GetElementSize() const   { return m_elementSize; }
        //uint GetElementCount() const  { return m_elementCount; }
        //
        //void SetElementSize(uint elementSize)     { m_elementSize = elementSize; }
        //void SetElementCount(uint elementCount)   { m_elementCount = elementCount; }
        //
        //bool  Init(void* initialData = 0);
        //void    Uninit();

    private:
        //ID3D11Buffer* m_buffer;

        //uint m_elementSize;
        //uint m_elementCount;
    };
}
}

#endif // CLOUD_RENDERER_GFX_SHADER_RESOURCE_HEADER