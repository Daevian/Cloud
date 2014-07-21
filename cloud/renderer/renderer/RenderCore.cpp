#include "stdafx.h"
#include "RenderCore.h"

#include "Settings.h"
#include "GfxTexture.h"

Cloud::Renderer::RenderCore* Cloud::Renderer::RenderCore::s_instance = 0;

Cloud::Renderer::RenderCore::RenderCore()
: m_device(nullptr)
, m_context(nullptr)
, m_swapChain(nullptr)
, m_renderTargetView(nullptr)
, m_depthStencilView(nullptr)
, m_depthStencilBuffer(nullptr)
, m_featureLevel(D3D_FEATURE_LEVEL_11_1)
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
        s_instance = new RenderCore();
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
        SAFE_DELETE(s_instance);
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

    m_context->OMSetRenderTargets( 1, &m_renderTargetView, m_depthStencilView );

    CL_TRACE_CHANNEL("INIT", "[RenderCore] Initialised!");
    return true;
}

void Cloud::Renderer::RenderCore::Shutdown()
{
    if( m_context ) m_context->ClearState();

    m_perSceneConstBuffer.Uninitialise();
    m_perModelConstBuffer.Uninitialise();
    if (m_depthStencilView) m_depthStencilView->Release();
    if (m_renderTargetView) m_renderTargetView->Release();
    if (m_swapChain) m_swapChain->Release();
    if (m_context) m_context->Release();
    if (m_device) m_device->Release();

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

    HRESULT result = D3D11CreateDeviceAndSwapChain( nullptr,
                                                    D3D_DRIVER_TYPE_HARDWARE,
                                                    0,
                                                    createDeviceFlags,
                                                    supportedFeatureLevels,
                                                    supportedFeatureLevelCount,
                                                    D3D11_SDK_VERSION,
                                                    &swapDesc,
                                                    &m_swapChain,
                                                    &m_device,
                                                    &m_featureLevel,
                                                    &m_context);

    
    if(FAILED(result))
    {
        CL_ASSERT_MSG("Failed to create swap chain!");
        CL_TRACE_CHANNEL("INIT", "[RenderCore] Failed to create swap chain!");
        return false;
    }

    return true;
}

CLbool Cloud::Renderer::RenderCore::InitBackBuffer()
{
    HRESULT result;

    ID3D11Texture2D* backBuffer = 0;
    result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if (FAILED(result))
    {
        if (backBuffer) backBuffer->Release();

        CL_ASSERT_MSG("Failed to get back buffer!");
        CL_TRACE_CHANNEL("INIT", "[RenderCore] Failed to get back buffer!");
        return false;
    }

    result = m_device->CreateRenderTargetView(backBuffer, 0, &m_renderTargetView);
    backBuffer->Release();
    
    if (FAILED(result))
    {
        CL_ASSERT_MSG("Failed to create back buffer RTV!");
        CL_TRACE_CHANNEL("INIT", "[RenderCore] Failed to create back buffer RTV!");
        return false;
    }

    return true;
}

CLbool Cloud::Renderer::RenderCore::InitDepthBuffer()
{
    auto& settings = Cloud::Renderer::Settings::Instance().GetRoot();

    D3D11_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Width     = settings["Resolution"]["Width"].asInt();
    depthStencilDesc.Height    = settings["Resolution"]["Height"].asInt();
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format    = (DXGI_FORMAT)GfxFormat::D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count   = settings["Graphics"]["MSAA"].asInt();
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags      = 0;

    HRESULT result;
    result = m_device->CreateTexture2D(&depthStencilDesc, 0, &m_depthStencilBuffer);
    if (FAILED(result))
    {
        if (m_depthStencilBuffer) m_depthStencilBuffer->Release();

        CL_ASSERT_MSG("Failed to create depth buffer!");
        CL_TRACE_CHANNEL("INIT", "[RenderCore] Failed to create depth buffer!");
        return false;
    }

    result = m_device->CreateDepthStencilView(m_depthStencilBuffer, 0, &m_depthStencilView);
    m_depthStencilBuffer->Release();

    if (FAILED(result))
    {
        CL_ASSERT_MSG("Failed to create depth stencil view!");
        CL_TRACE_CHANNEL("INIT", "[RenderCore] Failed to depth stencil view!");
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

    float clearColour[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    m_context->ClearRenderTargetView(m_renderTargetView, clearColour);
    m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
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

Cloud::Renderer::GfxBuffer* Cloud::Renderer::RenderCore::Create(const GfxBufferDesc& desc)
{
    return m_gfxBufferFactory.Create(desc);
}

Cloud::Renderer::GfxTexture* Cloud::Renderer::RenderCore::Create(const GfxTextureDesc& desc)
{
    return m_gfxTextureFactory.Create(desc);
}

Cloud::Renderer::GfxComputeShader* Cloud::Renderer::RenderCore::Create(const GfxComputerShaderDesc& desc)
{
    return m_gfxShaderFactory.Create(desc);
}

void Cloud::Renderer::RenderCore::Destroy(GfxBuffer* buffer)
{
    m_gfxBufferFactory.Destroy(buffer);
}

void Cloud::Renderer::RenderCore::Destroy(GfxTexture* texture)
{
    m_gfxTextureFactory.Destroy(texture);
}

void Cloud::Renderer::RenderCore::Destroy(GfxComputeShader* shader)
{
    m_gfxShaderFactory.Destroy(shader);
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
