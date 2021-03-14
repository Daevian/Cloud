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

#ifdef USE_DIRECTX12
    switch (desc.dim)
    {
        case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
            Init2d(desc, *texture);
            break;
        default:
            CL_ASSERT_MSG("dim not supported yet");
            break;
    }

    if (desc.flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
    {
        InitDsv(desc, *texture);
    }

    if (!(desc.flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE))
    {
        InitSrv(desc, *texture);
    }

#else

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
#endif

    return std::move(texture);
}

void Cloud::Renderer::GfxTextureFactory::Init2d(const GfxTextureDesc& desc, GfxTexture& texture)
{
#ifdef USE_DIRECTX12
    auto&& device = RenderCore::Instance().GetDevice();

    D3D12_RESOURCE_DESC dxDesc = {};
    dxDesc.MipLevels = static_cast<UINT16>(desc.mipCount);
    dxDesc.Format = desc.format;
    dxDesc.Width = desc.width;
    dxDesc.Height = desc.height;
    dxDesc.Flags = desc.flags;
    dxDesc.DepthOrArraySize = static_cast<UINT16>(desc.arraySize);
    dxDesc.SampleDesc.Count = desc.sampleDesc.Count;
    dxDesc.SampleDesc.Quality = desc.sampleDesc.Quality;
    dxDesc.Dimension = desc.dim;

    auto hasInitialData = desc.initialData.data != nullptr;

    auto canClear = (dxDesc.Flags & (static_cast<int>(D3D12_RESOURCE_DIMENSION_BUFFER) | static_cast<int>(D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) | static_cast<int>(D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL))) != 0;

    auto&& heapProperties = CD3DX12_HEAP_PROPERTIES(desc.heapType);
    if (FAILED(device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &dxDesc,
        hasInitialData ? D3D12_RESOURCE_STATE_COPY_DEST : desc.initialState,
        canClear ? &desc.clearValue : nullptr,
        IID_PPV_ARGS(&texture.m_resource))))
    {
        CL_ASSERT_MSG("Couldn't create texture!");
        return;
    }

    if (hasInitialData)
    {
        // Note: ComPtr's are CPU objects but this resource needs to stay in scope until
        // the command list that references it has finished executing on the GPU.
        // We will flush the GPU at the end of this method to ensure the resource is not
        // prematurely destroyed.
        ComPtr<ID3D12Resource> textureUploadHeap;

        auto&& uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto&& uploadBuffer = CD3DX12_RESOURCE_DESC::Buffer(desc.initialData.size);
        if (FAILED(device->CreateCommittedResource(
            &uploadHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &uploadBuffer,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&textureUploadHeap))))
        {
            CL_ASSERT_MSG("Couldn't create upload buffer!");
            return;
        }

        const auto c_texturePixelSize = sizeof(uint32);

        D3D12_SUBRESOURCE_DATA textureData = {};
        textureData.pData = desc.initialData.data;
        textureData.RowPitch = desc.width * c_texturePixelSize;
        textureData.SlicePitch = textureData.RowPitch * desc.height;
        auto&& commandList = RenderCore::Instance().AllocateCommandList();
        
        UpdateSubresources(commandList, texture.m_resource.Get(), textureUploadHeap.Get(), 0, 0, 1, &textureData);
        
        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(texture.m_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        commandList->ResourceBarrier(1, &barrier);

        if (FAILED(commandList->Close()))
        {
            CL_ASSERT_MSG("Failed to close command list!");
            return;
        }

        RenderCore::Instance().Flush();
    }

#else
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
        uint skipMip = 0;
        uint tWidth = 0;
        uint tHeight = 0;
        uint tDepth = 0;

        FillInitialData(
            desc.width,
            desc.height,
            desc.depth,
            desc.mipCount,
            desc.arraySize,
            desc.format,
            (uint8*)desc.initialData.data,
            desc.initialData.size,
            tWidth,
            tHeight,
            tDepth,
            skipMip,
            initData.get());
    }
    
    auto* device = GfxCore::Instance().GetDevice();
    ID3D11Texture2D* dxTex = nullptr;
    auto hr = device->CreateTexture2D(&dxDesc, desc.initialData.data ? initData.get() : nullptr, &dxTex);
    if (FAILED(hr))
    {
        CL_ASSERT_MSG("Failed to create the 2D Texture!");
        return;
    }

    texture.m_texture = Dx::MakeUnique(dxTex);

    RenderCore::SetDebugObjectName(texture.m_texture.get(), (desc.name + ".tex").c_str());
#endif
}

void Cloud::Renderer::GfxTextureFactory::InitSrv(const GfxTextureDesc& desc, GfxTexture& texture)
{
#ifdef USE_DIRECTX12
    auto&& device = RenderCore::Instance().GetDevice();

    static const auto c_srvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    const auto offset = RenderCore::c_cbvDescCount + RenderCore::Instance().m_srvHeapIndex++;
    texture.m_srv.InitOffsetted(RenderCore::Instance().GetCbvHeap()->GetCPUDescriptorHandleForHeapStart(), offset, c_srvDescriptorSize);

    D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
    viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    switch (texture.m_desc.format)
    {
        case DXGI_FORMAT_R32_TYPELESS:
            viewDesc.Format = DXGI_FORMAT_R32_FLOAT;
            break;
        default:
            viewDesc.Format = texture.m_desc.format;
            break;
    }

    switch (desc.dim)
    {
        case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
            viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            viewDesc.Texture2D.MostDetailedMip = 0;
            viewDesc.Texture2D.PlaneSlice = 0;
            viewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
            viewDesc.Texture2D.MipLevels = desc.mipCount;
            break;
        default:
            CL_ASSERT_MSG("dim not supported!");
            break;
    }

    CL_ASSERT(!desc.isCubeMap, "need to implement cubmap srvs");

    device->CreateShaderResourceView(texture.m_resource.Get(), &viewDesc, texture.m_srv);

#else
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
#endif
}

void Cloud::Renderer::GfxTextureFactory::InitRtv(const GfxTextureDesc& /*desc*/, GfxTexture& /*texture*/)
{
#ifdef USE_DIRECTX12
#else
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
#endif
}

void Cloud::Renderer::GfxTextureFactory::InitDsv(const GfxTextureDesc& desc, GfxTexture& texture)
{
#ifdef USE_DIRECTX12

    auto&& device = RenderCore::Instance().GetDevice();

    static auto s_descriptorIndex = 0;
    static const auto c_dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    texture.m_dsv.InitOffsetted(RenderCore::Instance().GetDsvHeap()->GetCPUDescriptorHandleForHeapStart(), s_descriptorIndex++, c_dsvDescriptorSize);

    D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc = {};

    switch (desc.format)
    {
        case DXGI_FORMAT_R32_TYPELESS:
            viewDesc.Format = DXGI_FORMAT_D32_FLOAT;
            break;
        default:
            CL_ASSERT_MSG("format not supported!");
            break;
    }

    switch (desc.dim)
    {
        case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
            viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            viewDesc.Texture2D.MipSlice = 0;
            break;
        default:
            CL_ASSERT_MSG("dim not supported!");
            break;
    }

    device->CreateDepthStencilView(texture.m_resource.Get(), &viewDesc, texture.m_dsv);

#else
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
#endif
}

#ifdef USE_DIRECTX12
#else
void Cloud::Renderer::GfxTextureFactory::FillInitialData(uint width, uint height, uint depth, uint mipCount, uint arraySize, DXGI_FORMAT format, const uint8* imageData, size_t imageDataSize, uint& tWidth, uint& tHeight, uint& tDepth, uint& skipMip, D3D11_SUBRESOURCE_DATA* initData)
{
    CL_ASSERT_NULL(imageData);
    CL_ASSERT_NULL(initData);

    skipMip = 0;
    tWidth = 0;
    tHeight = 0;
    tDepth = 0;

    uint NumBytes = 0;
    uint RowBytes = 0;
    uint NumRows = 0;
    const uint8* pSrcBits = imageData;
    const uint8* pEndBits = imageData + imageDataSize;

    uint index = 0;

    for (uint j = 0; j < arraySize; ++j)
    {
        uint w = width;
        uint h = height;
        uint d = depth;
        for (uint i = 0; i < mipCount; ++i)
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
#endif

Cloud::Renderer::GfxTexture::UniquePtr Cloud::Renderer::GfxTextureFactory::CreateFromBackbuffer()
{
#ifdef USE_DIRECTX12
    return nullptr;
#else
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
#endif
}
