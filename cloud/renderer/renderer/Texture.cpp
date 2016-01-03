#include "stdafx.h"
#include "Texture.h"
#include "RenderCore.h"



Cloud::Renderer::Texture::Texture(const std::string& texturePath)
    : m_texturePath(texturePath)
{
}

Cloud::Renderer::Texture::~Texture()
{
}

CLbool Cloud::Renderer::Texture::Load()
{
    if (!LoadResource()) return false;
    if (!LoadSampler()) return false;

    return true;
}

void Cloud::Renderer::Texture::Unload()
{
    CL_ASSERT(m_samplerState != 0, "Can't unload uninitialised texture!");

    m_texture = nullptr;
    m_samplerState = nullptr;
}

CLbool Cloud::Renderer::Texture::LoadResource()
{
    CL_ASSERT(!m_texture, (m_texturePath + " already loaded!").c_str());

    //load file
    std::unique_ptr<CLuint8[]> ddsData;
    DdsHeader* header;
    DdsHeaderDXT10* dxt10Header;
    CLuint8* imageData;
    CLsize_t imageDataSize;

    ReadTextureDataFromFile(m_texturePath, ddsData, header, dxt10Header, imageData, imageDataSize);
    CL_ASSERT(header && imageData && imageDataSize, "Didn't get valid data from the texture file");

    // set up texture data
    GfxTextureDesc desc;
    ClMemZero(&desc, sizeof(desc));
    desc.width = header->width;
    desc.height = header->height;
    desc.depth = header->depth;
    desc.dim = D3D11_RESOURCE_DIMENSION_UNKNOWN;
    desc.arraySize = 1;
    desc.format = DXGI_FORMAT_UNKNOWN;
    desc.isCubeMap = false;
    desc.usage = D3D11_USAGE_DEFAULT;
    desc.bindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.cpuAccessFlags = 0;
    desc.miscFlags = 0;
    desc.sampleDesc.Count = 1;
    desc.sampleDesc.Quality = 0;
    
    desc.mipCount = header->mipMapCount;
    if (desc.mipCount == 0)
    {
        desc.mipCount = 1;
    }
    
    if (dxt10Header)
    {
        CL_ASSERT_MSG("DXT10 not supported yet!");
    }
    else
    {
        desc.format = Cloud::Renderer::GetDXGIFormat(header->ddspf);
        CL_ASSERT(desc.format != DXGI_FORMAT_UNKNOWN, "Unknown format");
        
        if (header->flags & DDS_HEADER_FLAGS_VOLUME)
        {
            desc.dim = D3D11_RESOURCE_DIMENSION_TEXTURE3D;
        }
        else
        {
            if (header->caps2 & DDS_CUBEMAP)
            {
                if ((header->caps2 & DDS_CUBEMAP_ALLFACES) != DDS_CUBEMAP_ALLFACES)
                {
                    CL_ASSERT(desc.format != DXGI_FORMAT_UNKNOWN, "All six faces has to be present for the cube map to be valid");
                }

                desc.arraySize = 6;
                desc.isCubeMap = true;
            }

            desc.depth = 1;
            desc.dim = D3D11_RESOURCE_DIMENSION_TEXTURE2D;
        }
        
        CL_ASSERT(BitsPerPixel(desc.format) != 0, "Format is not valid");
    }

    desc.initialData.data = imageData;
    desc.initialData.size = imageDataSize;

    desc.name = m_texturePath;

    // create gfx texture
    m_texture = GfxCore::Instance().Create(desc);
    if (!m_texture)
    {
        CL_ASSERT_MSG(("Couldn't create texture " + m_texturePath).c_str());
        return false;
    }

    return true;
}

CLbool Cloud::Renderer::Texture::LoadSampler()
{
    D3D11_SAMPLER_DESC samplerDesc;
    ClMemZero(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    ID3D11SamplerState* dxSampler;
    auto result = RenderCore::Instance().GetDevice()->CreateSamplerState( &samplerDesc, &dxSampler);
    if (FAILED(result))
    {
        std::stringstream assertMessage;
        assertMessage << "Couldn't create sampler for " << m_texturePath;

        CL_TRACE_CHANNEL("ERROR", assertMessage.str().c_str());
        CL_ASSERT_MSG(assertMessage.str().c_str());
        return false;
    }

    m_samplerState = Dx::MakeUnique(dxSampler);

    return true;
}

void Cloud::Renderer::Texture::ReadTextureDataFromFile(const ClString& fileName, std::unique_ptr<CLuint8[]>& ddsData, DdsHeader*& header, DdsHeaderDXT10*& dxt10Header, CLuint8*& imageData, CLsize_t& imageDataSize)
{
    header = nullptr;
    dxt10Header = nullptr;
    imageData = nullptr;
    imageDataSize = 0;

    auto handle = Utils::File::LoadFile(fileName);
    if (!handle)
    {
        return;
    }
    
    auto fileSize = Utils::File::GetFileSize(handle);
    if (!fileSize)
    {
        return;
    }

    ddsData.reset(new CLuint8[fileSize]);
    CL_ASSERT_NULL(ddsData);

    Utils::File::Read(handle, ddsData, static_cast<CLuint>(fileSize));

    CL_ASSERT(IsDds(ddsData), "Only DDS is supported at the moment");
    
    // read DDS header
    header = reinterpret_cast<DdsHeader*>(ddsData.get() + sizeof(DDS_MAGIC));
    CL_ASSERT(header->size == sizeof(DdsHeader) && header->ddspf.size == sizeof(DdsPixelFormat), "Header size does not match the DDS header size");

    // check for DXT10 extension
    if ((header->ddspf.flags & DDS_FOURCC) &&
        (MAKEFOURCC('D', 'X', '1', '0') == header->ddspf.fourCC))
    {
        dxt10Header = reinterpret_cast<DdsHeaderDXT10*>(header + sizeof(DdsHeader));
        if (fileSize < (sizeof(DDS_MAGIC) + sizeof(DdsHeader) + sizeof(DdsHeaderDXT10)))
        {
            CL_ASSERT_MSG("Must be long enough for both headers and magic value");
        }
    }

    CLptrdiff_t dataOffset =
        sizeof(DDS_MAGIC) +
        sizeof(DdsHeader) +
        (dxt10Header ? sizeof(DdsHeaderDXT10) : 0);

    imageData = ddsData.get() + dataOffset;
    imageDataSize = fileSize - dataOffset;
}

CLbool Cloud::Renderer::Texture::IsDds(const std::unique_ptr<uint8_t[]>& ddsData)
{
    CLuint32 magicNumber = *(const CLuint32*)(ddsData.get());
    return magicNumber == DDS_MAGIC;
}