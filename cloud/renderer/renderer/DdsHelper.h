#ifndef CLOUD_RENDERER_DDS_HEADER
#define CLOUD_RENDERER_DDS_HEADER

#define DDS_FOURCC      0x00000004  // DDPF_FOURCC
#define DDS_RGB         0x00000040  // DDPF_RGB
#define DDS_LUMINANCE   0x00020000  // DDPF_LUMINANCE
#define DDS_ALPHA       0x00000002  // DDPF_ALPHA

#define DDS_HEADER_FLAGS_VOLUME         0x00800000  // DDSD_DEPTH

#define DDS_HEIGHT 0x00000002 // DDSD_HEIGHT
#define DDS_WIDTH  0x00000004 // DDSD_WIDTH

#define DDS_CUBEMAP_POSITIVEX 0x00000600 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX
#define DDS_CUBEMAP_NEGATIVEX 0x00000a00 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEX
#define DDS_CUBEMAP_POSITIVEY 0x00001200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEY
#define DDS_CUBEMAP_NEGATIVEY 0x00002200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEY
#define DDS_CUBEMAP_POSITIVEZ 0x00004200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEZ
#define DDS_CUBEMAP_NEGATIVEZ 0x00008200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEZ

#define DDS_CUBEMAP_ALLFACES ( DDS_CUBEMAP_POSITIVEX | DDS_CUBEMAP_NEGATIVEX |\
                               DDS_CUBEMAP_POSITIVEY | DDS_CUBEMAP_NEGATIVEY |\
                               DDS_CUBEMAP_POSITIVEZ | DDS_CUBEMAP_NEGATIVEZ )

#define DDS_CUBEMAP 0x00000200 // DDSCAPS2_CUBEMAP

#define ISBITMASK( r,g,b,a ) ( ddpf.RBitMask == r && ddpf.GBitMask == g && ddpf.BBitMask == b && ddpf.ABitMask == a )

namespace Cloud
{
    namespace Renderer
    {
        struct DdsPixelFormat
        {
            uint32    size;
            uint32    flags;
            uint32    fourCC;
            uint32    RGBBitCount;
            uint32    RBitMask;
            uint32    GBitMask;
            uint32    BBitMask;
            uint32    ABitMask;
        };

        struct DdsHeader
        {
            uint32        size;
            uint32        flags;
            uint32        height;
            uint32        width;
            uint32        pitchOrLinearSize;
            uint32        depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
            uint32        mipMapCount;
            uint32        reserved1[11];
            DdsPixelFormat  ddspf;
            uint32        caps;
            uint32        caps2;
            uint32        caps3;
            uint32        caps4;
            uint32        reserved2;
        };

        struct DdsHeaderDXT10
        {
            DXGI_FORMAT     dxgiFormat;
            uint32        resourceDimension;
            uint32        miscFlag; // see D3D11_RESOURCE_MISC_FLAG
            uint32        arraySize;
            uint32        miscFlags2;
        };

        static const uint32 DDS_MAGIC = 0x20534444; // "DDS "
        
        DXGI_FORMAT GetDXGIFormat(const DdsPixelFormat& ddpf);
    }
}

#endif // CLOUD_RENDERER_DDS_HEADER