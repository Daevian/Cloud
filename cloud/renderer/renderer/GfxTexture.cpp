#include "stdafx.h"
#include "GfxTexture.h"

#include "RenderCore.h"

Cloud::Renderer::GfxTexture::GfxTexture()
    : GfxResource()
{
    ClMemZero(&m_desc, sizeof(m_desc));
}

Cloud::Renderer::GfxTexture::~GfxTexture()
{
}

Cloud::Renderer::GfxTexture::UniquePtr Cloud::Renderer::GfxTextureFactory::Create(const GfxTextureDesc& desc)
{
    auto texture = GfxTexture::MakeUnique();

    texture->m_desc = desc;

    switch (desc.dim)
    {
    case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
        Init2d(desc, *texture);
        break;
    default:
        CL_ASSERT_MSG("dim not supported yet");
        break;
    }

    if (desc.bindFlags & D3D11_BIND_SHADER_RESOURCE)
    {
        InitSrv(desc, *texture);
    }

    if (desc.bindFlags & D3D11_BIND_RENDER_TARGET)
    {
        InitRtv(desc, *texture);
    }

    if (desc.bindFlags & D3D11_BIND_DEPTH_STENCIL)
    {
        InitDsv(desc, *texture);
    }

    return std::move(texture);
}

void Cloud::Renderer::GfxTextureFactory::Init2d(const GfxTextureDesc& desc, GfxTexture& texture)
{
    CL_ASSERT(desc.dim == D3D11_RESOURCE_DIMENSION_TEXTURE2D, "dim has to be 2d for 2d textures");
    CL_ASSERT(!texture.m_texture, "The texture has already been created!");

    D3D11_TEXTURE2D_DESC dxDesc;
    dxDesc.Width = desc.width;
    dxDesc.Height = desc.height;
    dxDesc.MipLevels = desc.mipCount;
    dxDesc.ArraySize = desc.arraySize;
    dxDesc.Format = desc.format;
    dxDesc.SampleDesc.Count = desc.sampleDesc.Count;
    dxDesc.SampleDesc.Quality = desc.sampleDesc.Quality;
    dxDesc.Usage = desc.usage;
    dxDesc.BindFlags = desc.bindFlags;
    dxDesc.CPUAccessFlags = desc.cpuAccessFlags;
    dxDesc.MiscFlags = desc.miscFlags;
    if (desc.isCubeMap)
    {
        dxDesc.MiscFlags |= desc.miscFlags | D3D11_RESOURCE_MISC_TEXTURECUBE;
        CL_ASSERT(dxDesc.ArraySize <= D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION, "Texture has too many array pages!");
        CL_ASSERT(dxDesc.Width <= D3D11_REQ_TEXTURECUBE_DIMENSION, "Texture is too big!");
        CL_ASSERT(dxDesc.Height <= D3D11_REQ_TEXTURECUBE_DIMENSION, "Texture is too big!");
    }
    else
    {
        dxDesc.MiscFlags |= desc.miscFlags & ~D3D11_RESOURCE_MISC_TEXTURECUBE;
        CL_ASSERT(dxDesc.ArraySize <= D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION, "Texture has too many array pages!");
        CL_ASSERT(dxDesc.Width <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION, "Texture is too big!");
        CL_ASSERT(dxDesc.Height <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION, "Texture is too big!");
    }

    CL_ASSERT(dxDesc.MipLevels <= D3D11_REQ_MIP_LEVELS, "Too many mip levels for DX11.x");

    std::unique_ptr<D3D11_SUBRESOURCE_DATA[]> initData(new D3D11_SUBRESOURCE_DATA[desc.mipCount * desc.arraySize]);
    CL_ASSERT_NULL(initData);

    if (desc.initialData.data)
    {
        CLuint skipMip = 0;
        CLuint tWidth = 0;
        CLuint tHeight = 0;
        CLuint tDepth = 0;

        FillInitialData(
            desc.width,
            desc.height,
            desc.depth,
            desc.mipCount,
            desc.arraySize,
            desc.format,
            (CLuint8*)desc.initialData.data,
            desc.initialData.size,
            tWidth,
            tHeight,
            tDepth,
            skipMip,
            initData.get());
    }
    
    auto* device = GfxCore::Instance().GetDevice();
    ID3D11Texture2D* dxTex;
    auto hr = device->CreateTexture2D(&dxDesc, desc.initialData.data ? initData.get() : nullptr, &dxTex);
    if (FAILED(hr))
    {
        CL_ASSERT_MSG("Failed to create the 2D Texture!");
        return;
    }

    texture.m_texture = Dx::MakeUnique(dxTex);

    RenderCore::SetDebugObjectName(texture.m_texture.get(), (desc.name + ".tex").c_str());
}

void Cloud::Renderer::GfxTextureFactory::InitSrv(const GfxTextureDesc& desc, GfxTexture& texture)
{
    CL_ASSERT_NULL(texture.m_texture);
    CL_ASSERT(!texture.m_srv, "The srv has already been created!");


    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ClMemZero(&srvDesc, sizeof(srvDesc));

	switch (texture.m_desc.format)
	{
	case DXGI_FORMAT_R32_TYPELESS:
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		break;
	default:
		srvDesc.Format = texture.m_desc.format;
		break;
	}

    if (desc.isCubeMap)
    {
        if (desc.arraySize > 6)
        {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
            srvDesc.TextureCubeArray.MipLevels = desc.mipCount;
            srvDesc.TextureCubeArray.NumCubes = desc.arraySize / 6;
        }
        else
        {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            srvDesc.TextureCube.MipLevels = desc.mipCount;
        }
    }
    else if (desc.arraySize > 1)
    {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Texture2DArray.MipLevels = desc.mipCount;
        srvDesc.Texture2DArray.ArraySize = desc.arraySize;
    }
    else
    {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.mipCount;
    }

    auto* device = GfxCore::Instance().GetDevice();
    ID3D11ShaderResourceView* dxSrv;
    auto hr = device->CreateShaderResourceView(texture.m_texture.get(), &srvDesc, &dxSrv);
    if (FAILED(hr))
    {
        CL_ASSERT_MSG("Failed to create the SRV!");
        return;
    }

    texture.m_srv = Dx::MakeUnique(dxSrv);

    RenderCore::SetDebugObjectName(texture.m_srv.get(), (desc.name + ".srv").c_str());
}

void Cloud::Renderer::GfxTextureFactory::InitRtv(const GfxTextureDesc& desc, GfxTexture& texture)
{
    //D3D11_RENDER_TARGET_VIEW_DESC desc;
    //desc.

    ID3D11RenderTargetView* dxRtv;
    auto result = GfxCore::Instance().GetDevice()->CreateRenderTargetView(texture.m_texture.get(), nullptr, &dxRtv);
    if (FAILED(result))
    {
        CL_ASSERT_MSG("Failed to create the RTV!");
        return;
    }

    texture.m_rtv = Dx::MakeUnique(dxRtv);

    RenderCore::SetDebugObjectName(texture.m_rtv.get(), (desc.name + ".rtv").c_str());
}

void Cloud::Renderer::GfxTextureFactory::InitDsv(const GfxTextureDesc& desc, GfxTexture& texture)
{
	D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc;
	depthDesc.Flags = 0;

	switch (desc.format)
	{
	case DXGI_FORMAT_R32_TYPELESS:
		depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
		break;
	default:
		CL_ASSERT_MSG("format not supported!");
		break;
	}

	switch (desc.dim)
	{
	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthDesc.Texture2D.MipSlice = 0;
		break;
	default:
		CL_ASSERT_MSG("dim not supported!");
		break;
	}

    ID3D11DepthStencilView* dxDsv;
    auto result = GfxCore::Instance().GetDevice()->CreateDepthStencilView(texture.m_texture.get(), &depthDesc, &dxDsv);
    if (FAILED(result))
    {
        CL_ASSERT_MSG("Failed to create the DSV!");
        return;
    }

    texture.m_dsv = Dx::MakeUnique(dxDsv);

    RenderCore::SetDebugObjectName(texture.m_dsv.get(), (desc.name + ".dsv").c_str());
}

void Cloud::Renderer::GfxTextureFactory::FillInitialData(CLuint width, CLuint height, CLuint depth, CLuint mipCount, CLuint arraySize, DXGI_FORMAT format, const CLuint8* imageData, CLsize_t imageDataSize, CLuint& tWidth, CLuint& tHeight, CLuint& tDepth, CLuint& skipMip, D3D11_SUBRESOURCE_DATA* initData)
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

Cloud::Renderer::GfxTexture::UniquePtr Cloud::Renderer::GfxTextureFactory::CreateFromBackbuffer()
{
    auto texture = GfxTexture::MakeUnique();

    {
        ID3D11Texture2D* backbuffer = 0;
        auto result = GfxCore::Instance().GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);
        if (FAILED(result))
        {
            CL_ASSERT_MSG("Failed to get back buffer!");
        }

        texture->m_texture = Dx::MakeUnique(backbuffer);

        D3D11_TEXTURE2D_DESC desc;
        backbuffer->GetDesc(&desc);
        texture->m_desc.name                = "backbuffer";
        texture->m_desc.dim                 = D3D11_RESOURCE_DIMENSION_TEXTURE2D;
        texture->m_desc.isCubeMap           = false;
        texture->m_desc.width               = desc.Width;
        texture->m_desc.height              = desc.Height;
        texture->m_desc.mipCount            = desc.MipLevels;
        texture->m_desc.arraySize           = desc.ArraySize;
        texture->m_desc.format              = desc.Format;
        texture->m_desc.bindFlags           = desc.BindFlags;
        texture->m_desc.cpuAccessFlags      = desc.CPUAccessFlags;
        texture->m_desc.miscFlags           = desc.MiscFlags;
        texture->m_desc.sampleDesc.Count    = desc.SampleDesc.Count;
        texture->m_desc.sampleDesc.Quality  = desc.SampleDesc.Quality;

        RenderCore::SetDebugObjectName(texture->m_texture.get(), (texture->m_desc.name + ".tex").c_str());
    }

    // TODO: NEEDS MS SRV!
    //if (texture->m_desc.bindFlags & D3D11_BIND_SHADER_RESOURCE)
    //{
    //    InitSrv(texture->m_desc, *texture);
    //}

    if (texture->m_desc.bindFlags & D3D11_BIND_RENDER_TARGET)
    {
        InitRtv(texture->m_desc, *texture);
    }

    return texture;
}
