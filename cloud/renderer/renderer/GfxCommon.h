#ifndef CLOUD_RENDERER_GFX_COMMON_HEADER
#define CLOUD_RENDERER_GFX_COMMON_HEADER

#include <d3d11.h>

namespace Cloud
{
    namespace Renderer
    {
        enum class GfxPrimitiveTopology
        {
            Undefined = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED,
            Pointlist = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
            Linelist = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
            Linestrip = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
            Trianglelist = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
            Trianglestrip = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
            Linelist_adj = D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
            Linestrip_adj = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
            Trianglelist_adj = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
            Trianglestrip_adj = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
        };
    }
}

#endif // CLOUD_RENDERER_GFX_COMMON_HEADER

