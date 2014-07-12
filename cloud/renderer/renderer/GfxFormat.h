#ifndef CLOUD_RENDERER_GFX_FORMAT_HEADER
#define CLOUD_RENDERER_GFX_FORMAT_HEADER

namespace Cloud
{
namespace Renderer
{
    enum class GfxFormat
    {
        UNKNOWN = DXGI_FORMAT_UNKNOWN,
        R32G32_FLOAT = DXGI_FORMAT_R32G32_FLOAT,
        R32G32B32_FLOAT = DXGI_FORMAT_R32G32B32_FLOAT,
        R32G32B32A32_FLOAT = DXGI_FORMAT_R32G32B32A32_FLOAT,

        D24_UNORM_S8_UINT = DXGI_FORMAT_D24_UNORM_S8_UINT,
    };

    GfxFormat   GetFormat(const std::string& formatName);
    CLuint      GetFormatSize(GfxFormat format);
}
}

#endif // CLOUD_RENDERER_GFX_FORMAT_HEADER

