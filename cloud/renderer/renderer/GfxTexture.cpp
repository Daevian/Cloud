#include "stdafx.h"
#include "GfxTexture.h"

#include "RenderCore.h"

Cloud::Renderer::GfxTexture::GfxTexture()
    : m_texture(nullptr)
    , m_srv(nullptr)
{
    ClMemZero(&m_desc, sizeof(m_desc));
}

Cloud::Renderer::GfxTexture::~GfxTexture()
{
    if (m_texture)
    {
        m_texture->Release();
    }

    if (m_srv)
    {
        m_srv->Release();
    }
}

void Cloud::Renderer::GfxTexture::Init(const GfxTextureDesc& desc)
{
    m_desc = desc;

    switch (desc.dim)
    {
    case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
        Init2d();
        break;
    default:
        CL_ASSERT_MSG("dim not supported yet");
        break;
    }

    if (m_texture)
    {
        InitSrv();
    }
}

inline void SetDebugObjectName(ID3D11DeviceChild* resource, const CLchar* name)
{
#if defined(_DEBUG) || defined(PROFILE)
    resource->SetPrivateData(WKPDID_D3DDebugObjectName, (CLuint)ClStrlen(name), name);
#else
    CL_UNUSED(resource);
    CL_UNUSED(name);
#endif
}

void Cloud::Renderer::GfxTexture::Init2d()
{
    CL_ASSERT(m_desc.dim == D3D11_RESOURCE_DIMENSION_TEXTURE2D, "dim has to be 2d for 2d textures");
    CL_ASSERT(!m_texture, "The texture has already been created!");

    D3D11_TEXTURE2D_DESC dxDesc;
    dxDesc.Width = m_desc.width;
    dxDesc.Height = m_desc.height;
    dxDesc.MipLevels = m_desc.mipCount;
    dxDesc.ArraySize = m_desc.arraySize;
    dxDesc.Format = m_desc.format;
    dxDesc.SampleDesc.Count = 1;
    dxDesc.SampleDesc.Quality = 0;
    dxDesc.Usage = m_desc.usage;
    dxDesc.BindFlags = m_desc.bindFlags;
    dxDesc.CPUAccessFlags = m_desc.cpuAccessFlags;
    if (m_desc.isCubeMap)
    {
        dxDesc.MiscFlags = m_desc.miscFlags | D3D11_RESOURCE_MISC_TEXTURECUBE;
        CL_ASSERT(dxDesc.ArraySize <= D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION, "Texture has too many array pages!");
        CL_ASSERT(dxDesc.Width <= D3D11_REQ_TEXTURECUBE_DIMENSION, "Texture is too big!");
        CL_ASSERT(dxDesc.Height <= D3D11_REQ_TEXTURECUBE_DIMENSION, "Texture is too big!");
    }
    else
    {
        dxDesc.MiscFlags = m_desc.miscFlags & ~D3D11_RESOURCE_MISC_TEXTURECUBE;
        CL_ASSERT(dxDesc.ArraySize <= D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION, "Texture has too many array pages!");
        CL_ASSERT(dxDesc.Width <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION, "Texture is too big!");
        CL_ASSERT(dxDesc.Height <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION, "Texture is too big!");
    }

    CL_ASSERT(dxDesc.MipLevels <= D3D11_REQ_MIP_LEVELS, "Too many mip levels for DX11.x");

    std::unique_ptr<D3D11_SUBRESOURCE_DATA[]> initData(new D3D11_SUBRESOURCE_DATA[m_desc.mipCount * m_desc.arraySize]);
    CL_ASSERT_NULL(initData);

    if (m_desc.initialData.data)
    {
        CLuint skipMip = 0;
        CLuint tWidth = 0;
        CLuint tHeight = 0;
        CLuint tDepth = 0;

        FillInitialData(
            m_desc.width,
            m_desc.height,
            m_desc.depth,
            m_desc.mipCount,
            m_desc.arraySize,
            m_desc.format,
            (CLuint8*)m_desc.initialData.data,
            m_desc.initialData.size,
            tWidth,
            tHeight,
            tDepth,
            skipMip,
            initData.get());
    }
    
    auto* device = GfxCore::Instance().GetDevice();
    auto hr = device->CreateTexture2D(&dxDesc, initData.get(), &m_texture);
    if (FAILED(hr))
    {
        CL_ASSERT_MSG("Failed to create the 2D Texture!");
    }

    SetDebugObjectName(m_texture, (m_desc.name + ".tex").c_str());
}

void Cloud::Renderer::GfxTexture::InitSrv()
{
    CL_ASSERT_NULL(m_texture);
    CL_ASSERT(!m_srv, "The srv has already been created!");


    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ClMemZero(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = m_desc.format;

    if (m_desc.isCubeMap)
    {
        if (m_desc.arraySize > 6)
        {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
            srvDesc.TextureCubeArray.MipLevels = m_desc.mipCount;
            srvDesc.TextureCubeArray.NumCubes = m_desc.arraySize / 6;
        }
        else
        {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            srvDesc.TextureCube.MipLevels = m_desc.mipCount;
        }
    }
    else if (m_desc.arraySize > 1)
    {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Texture2DArray.MipLevels = m_desc.mipCount;
        srvDesc.Texture2DArray.ArraySize = m_desc.arraySize;
    }
    else
    {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = m_desc.mipCount;
    }

    auto* device = GfxCore::Instance().GetDevice();
    auto hr = device->CreateShaderResourceView(m_texture, &srvDesc, &m_srv);
    if (FAILED(hr))
    {
        CL_ASSERT_MSG("Failed to create the SRV!");
    }

    SetDebugObjectName(m_srv, (m_desc.name + ".srv").c_str());
}

void Cloud::Renderer::GfxTexture::Destroy()
{
    if (m_srv)
    {
        m_srv->Release();
    }

    if (m_texture)
    {
        m_texture->Release();
    }

    ClMemZero(&m_desc, sizeof(m_desc));
}

void Cloud::Renderer::GfxTexture::FillInitialData(CLuint width, CLuint height, CLuint depth, CLuint mipCount, CLuint arraySize, DXGI_FORMAT format, const CLuint8* imageData, CLsize_t imageDataSize, CLuint& tWidth, CLuint& tHeight, CLuint& tDepth, CLuint& skipMip, D3D11_SUBRESOURCE_DATA* initData)
{
    CL_ASSERT_NULL(imageData);
    CL_ASSERT_NULL(initData);

    skipMip = 0;
    tWidth = 0;
    tHeight = 0;
    tDepth = 0;

    CLuint NumBytes = 0;
    CLuint RowBytes = 0;
    CLuint NumRows = 0;
    const CLuint8* pSrcBits = imageData;
    const CLuint8* pEndBits = imageData + imageDataSize;

    CLuint index = 0;

    for (CLuint j = 0; j < arraySize; ++j)
    {
        CLuint w = width;
        CLuint h = height;
        CLuint d = depth;
        for (CLuint i = 0; i < mipCount; ++i)
        {
            GetSurfaceInfo(
                w,
                h,
                format,
                &NumBytes,
                &RowBytes,
                &NumRows);

            if (mipCount <= 1)
            {
                if (!tWidth)
                {
                    tWidth = w;
                    tHeight = h;
                    tDepth = d;
                }

                CL_ASSERT(index < mipCount * arraySize, "Not enough pages");
                initData[index].pSysMem = ( const void* )pSrcBits;
                initData[index].SysMemPitch = static_cast<UINT>( RowBytes );
                initData[index].SysMemSlicePitch = static_cast<UINT>( NumBytes );
                ++index;
            }
            else if (!j)
            {
                // Count number of skipped mipmaps (first item only)
                ++skipMip;
            }

            if (pSrcBits + (NumBytes*d) > pEndBits)
            {
                CL_ASSERT_MSG("End of file");
                return;
            }

            pSrcBits += NumBytes * d;

            w = w >> 1;
            h = h >> 1;
            d = d >> 1;
            if (w == 0)
            {
                w = 1;
            }
            if (h == 0)
            {
                h = 1;
            }
            if (d == 0)
            {
                d = 1;
            }
        }
    }

    CL_ASSERT(index > 0, "All surface copies failed!");
}
