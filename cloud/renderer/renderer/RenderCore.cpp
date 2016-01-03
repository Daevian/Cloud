#include "stdafx.h"
#include "RenderCore.h"

#include "Settings.h"
#include "GfxTexture.h"

std::unique_ptr<Cloud::Renderer::RenderCore, Cloud::Renderer::RenderCore::Deleter> Cloud::Renderer::RenderCore::s_instance = nullptr;

Cloud::Renderer::RenderCore::RenderCore()
    : m_featureLevel(D3D_FEATURE_LEVEL_11_1)
{
    ClMemZero(&m_settings, sizeof(m_settings));
}

Cloud::Renderer::RenderCore::~RenderCore()
{
}

CLbool Cloud::Renderer::RenderCore::Create(const Settings& settings)
{
    CL_ASSERT(s_instance == 0, "RenderCore already created. Can't re-create!");

    if (s_instance == 0)
    {
        s_instance = std::unique_ptr<RenderCore, Deleter>(new RenderCore());
        return s_instance->Initialise(settings);
    }

    return false;
}

void Cloud::Renderer::RenderCore::Destroy()
{
    CL_ASSERT(s_instance != 0, "RenderCore not created yet. Can't destroy!");

    if (s_instance != 0)
    {
        s_instance->Shutdown();
        s_instance = nullptr;
    }
}

CLbool Cloud::Renderer::RenderCore::Initialise(const Settings& settings)
{
    m_settings = settings;

    if (!InitSwapChain()) { return false; }
    if (!InitBackBuffer()) { return false; }
    if (!InitDepthBuffer()) { return false; }
    if (!InitConstantBuffers()) { return false; }
    InitViewPort();

    CL_TRACE_CHANNEL("INIT", "[RenderCore] Initialised!");
    return true;
}

void Cloud::Renderer::RenderCore::Shutdown()
{
    if (m_context) m_context->ClearState();

    m_perSceneConstBuffer.Uninitialise();
    m_perModelConstBuffer.Uninitialise();
    m_depthStencil = nullptr;
    m_backbuffer = nullptr;
    m_swapChain = nullptr;
    m_context = nullptr;
    m_device = nullptr;

    CL_TRACE_CHANNEL("INIT", "[RenderCore] Shut down!");
}

CLbool Cloud::Renderer::RenderCore::InitSwapChain()
{
    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    auto& settings = Cloud::Renderer::Settings::Instance().GetRoot();

    DXGI_SWAP_CHAIN_DESC swapDesc;
    ClMemZero(&swapDesc, sizeof(swapDesc));
    swapDesc.BufferCount = 1;
    swapDesc.BufferDesc.Width = settings["Resolution"]["Width"].asInt();
    swapDesc.BufferDesc.Height = settings["Resolution"]["Height"].asInt();
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
    swapDesc.OutputWindow = m_settings.m_hWnd;
    swapDesc.SampleDesc.Count = settings["Graphics"]["MSAA"].asInt();
    swapDesc.SampleDesc.Quality = 0;
    swapDesc.Windowed = true;

    D3D_FEATURE_LEVEL supportedFeatureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    CLuint supportedFeatureLevelCount = sizeof(supportedFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);
    ID3D11Device* device;
    IDXGISwapChain* swapChain;
    ID3D11DeviceContext* context;
    HRESULT result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        0,
        createDeviceFlags,
        supportedFeatureLevels,
        supportedFeatureLevelCount,
        D3D11_SDK_VERSION,
        &swapDesc,
        &swapChain,
        &device,
        &m_featureLevel,
        &context);

    if (FAILED(result))
    {
        CL_ASSERT_MSG("Failed to create swap chain!");
        CL_TRACE_CHANNEL("INIT", "[RenderCore] Failed to create swap chain!");
        return false;
    }

    m_device = Dx::MakeUnique(device);
    m_swapChain = Dx::MakeUnique(swapChain);
    m_context = Dx::MakeUnique(context);

    return true;
}

CLbool Cloud::Renderer::RenderCore::InitBackBuffer()
{
    m_backbuffer = m_gfxTextureFactory.CreateFromBackbuffer();
    if (!m_backbuffer)
    {
        CL_ASSERT_MSG("Failed to create back buffer!");
        CL_TRACE_CHANNEL("INIT", "[RenderCore] Failed to create back buffer!");
        return false;
    }

    return true;
}

CLbool Cloud::Renderer::RenderCore::InitDepthBuffer()
{
    auto& settings = Cloud::Renderer::Settings::Instance().GetRoot();

    GfxTextureDesc desc;
    ClMemZero(&desc, sizeof(desc));
    desc.name                   = "depthstencil";
    desc.dim                    = D3D11_RESOURCE_DIMENSION_TEXTURE2D;
    desc.width                  = settings["Resolution"]["Width"].asInt();
    desc.height                 = settings["Resolution"]["Height"].asInt();
    desc.mipCount               = 1;
    desc.arraySize              = 1;
    desc.format                 = (DXGI_FORMAT)GfxFormat::R32_TYPELESS;
    desc.sampleDesc.Count       = settings["Graphics"]["MSAA"].asInt();
    desc.sampleDesc.Quality     = 0;
    desc.usage                  = D3D11_USAGE_DEFAULT;
	desc.bindFlags				= D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    desc.cpuAccessFlags         = 0;
    desc.miscFlags              = 0;

    m_depthStencil = Create(desc);
    if (!m_depthStencil)
    {
        CL_ASSERT_MSG("Failed to create depth stencil!");
        CL_TRACE_CHANNEL("INIT", "[RenderCore] Failed to depth stencil!");
        return false;
    }

    return true;
}

CLbool Cloud::Renderer::RenderCore::InitConstantBuffers()
{
    m_perSceneConstBuffer.SetData(&m_perSceneConstData, sizeof(PerSceneConstBuffer));
    if (!m_perSceneConstBuffer.Initialise())
        return false;
    
    m_perModelConstBuffer.SetData(&m_perModelConstData, sizeof(PerModelConstBuffer));
    if (!m_perModelConstBuffer.Initialise())
        return false;

    return true;
}

void Cloud::Renderer::RenderCore::InitViewPort()
{
    m_viewPort.Width = static_cast<CLfloat>(Cloud::Renderer::Settings::Instance().GetRoot()["Resolution"]["Width"].asDouble());
    m_viewPort.Height = static_cast<CLfloat>(Cloud::Renderer::Settings::Instance().GetRoot()["Resolution"]["Height"].asDouble());
    m_viewPort.MinDepth = 0.0f;
    m_viewPort.MaxDepth = 1.0f;
    m_viewPort.TopLeftX = 0;
    m_viewPort.TopLeftY = 0;
    m_context->RSSetViewports(1, &m_viewPort);
}

void Cloud::Renderer::RenderCore::GpuUpdatePerSceneConstBuffer()
{
    m_renderingDevice.SetConstantBuffer(&m_perSceneConstBuffer, 0);
    m_perSceneConstBuffer.GPUUpdateConstantBuffer();
}

void Cloud::Renderer::RenderCore::GpuUpdatePerModelConstBuffer()
{
    m_renderingDevice.SetConstantBuffer(&m_perModelConstBuffer, 1);
    m_perModelConstBuffer.GPUUpdateConstantBuffer();
}

void Cloud::Renderer::RenderCore::Present()
{
    const CLbool enableVSync = Cloud::Renderer::Settings::Instance().GetRoot()["Graphics"]["VSync"].asBool();
    m_swapChain->Present(enableVSync ? 1 : 0, 0);
}

CLuint Cloud::Renderer::RenderCore::GetMSAAQuality(CLuint samples, DXGI_FORMAT format)
{
    CLuint quality = 1;
    HRESULT result = m_device->CheckMultisampleQualityLevels(format, samples, &quality);
    if (result)
    {
        CL_ASSERT_MSG("Failed to get MSAA quality on format!");
    }

    return quality - 1;
}

Cloud::Renderer::GfxBuffer::UniquePtr Cloud::Renderer::RenderCore::Create(const GfxBufferDesc& desc)
{
    return m_gfxBufferFactory.Create(desc);
}

Cloud::Renderer::GfxTexture::UniquePtr Cloud::Renderer::RenderCore::Create(const GfxTextureDesc& desc)
{
    return m_gfxTextureFactory.Create(desc);
}

Cloud::Renderer::GfxComputeShader::UniquePtr Cloud::Renderer::RenderCore::Create(const GfxComputerShaderDesc& desc)
{
    return m_gfxShaderFactory.Create(desc);
}

void Cloud::Renderer::RenderCore::SetDebugObjectName(ID3D11DeviceChild* resource, const CLchar* name)
{
    {
#if defined(_DEBUG) || defined(PROFILE)
        resource->SetPrivateData(WKPDID_D3DDebugObjectName, (CLuint)ClStrlen(name), name);
#else
        CL_UNUSED(resource);
        CL_UNUSED(name);
#endif
    }
}
