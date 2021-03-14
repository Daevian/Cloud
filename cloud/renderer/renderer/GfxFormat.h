#ifndef CLOUD_RENDERER_GFX_FORMAT_HEADER
#define CLOUD_RENDERER_GFX_FORMAT_HEADER

namespace Cloud
{
    namespace Renderer
    {
        enum class GfxFormat
        {
            UNKNOWN = DXGI_FORMAT_UNKNOWN,

            R8G8B8A8_UNORM = DXGI_FORMAT_R8G8B8A8_UNORM,

            R32G32_FLOAT = DXGI_FORMAT_R32G32_FLOAT,
            R32G32B32_FLOAT = DXGI_FORMAT_R32G32B32_FLOAT,
            R32G32B32A32_FLOAT = DXGI_FORMAT_R32G32B32A32_FLOAT,


			R32_TYPELESS = DXGI_FORMAT_R32_TYPELESS,
			D32_FLOAT = DXGI_FORMAT_D32_FLOAT,
            D24_UNORM_S8_UINT = DXGI_FORMAT_D24_UNORM_S8_UINT,
        };

        GfxFormat   GetFormat(const std::string& formatName);
        uint      GetFormatSize(GfxFormat format);
        uint      BitsPerPixel(DXGI_FORMAT format);
        void        GetSurfaceInfo( uint width,
                                    uint height,
                                    DXGI_FORMAT fmt,
                                    uint* outNumBytes,
                                    uint* outRowBytes,
                                    uint* outNumRows );
    }
}

#endif // CLOUD_RENDERER_GFX_FORMAT_HEADER

