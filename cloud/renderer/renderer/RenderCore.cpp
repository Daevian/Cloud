#include "stdafx.h"
#include "RenderCore.h"

#include "Settings.h"

Cloud::Renderer::RenderCore* Cloud::Renderer::RenderCore::s_instance = 0;

Cloud::Renderer::RenderCore::RenderCore()
: m_device(0)
, m_deviceContext(0)
, m_swapChain(0)
, m_renderTargetView(0)
, m_featureLevel(D3D_FEATURE_LEVEL_11_0)
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
    InitViewPort();

    if (!m_renderingDevice.Init()) { return false; }

    CL_TRACE_CHANNEL("INIT", "[RenderCore] Initialised!");
    return true;
}

void Cloud::Renderer::RenderCore::Shutdown()
{
    if( m_deviceContext ) m_deviceContext->ClearState();

    if (m_renderTargetView) m_renderTargetView->Release();
    if (m_swapChain) m_swapChain->Release();
    if (m_deviceContext) m_deviceContext->Release();
    if (m_device) m_device->Release();

    CL_TRACE_CHANNEL("INIT", "[RenderCore] Shut down!");
}

CLbool Cloud::Renderer::RenderCore::InitSwapChain()
{
    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    DXGI_SWAP_CHAIN_DESC swapDesc;
    ClMemZero(&swapDesc, sizeof(swapDesc));
    swapDesc.BufferCount = 1;
    swapDesc.BufferDesc.Width = Cloud::Renderer::Settings::Instance().GetRoot()["Resolution"]["Width"].asInt();
    swapDesc.BufferDesc.Height = Cloud::Renderer::Settings::Instance().GetRoot()["Resolution"]["Height"].asInt();
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
    swapDesc.OutputWindow = m_settings.m_hWnd;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.SampleDesc.Quality = 0;
    swapDesc.Windowed = true;

    IDXGIAdapter* selectedAdapter = 0;
    D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

//#ifndef _MASTER
//    IDXGIFactory *pDXGIFactory;
//    HRESULT hRes;
//    hRes = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pDXGIFactory);
//
//    UINT nAdapter = 0;
//    IDXGIAdapter* adapter = 0;
//    while (pDXGIFactory->EnumAdapters(nAdapter, &adapter) != DXGI_ERROR_NOT_FOUND)
//    {
//        if (adapter)
//        {
//            DXGI_ADAPTER_DESC adaptDesc;
//            if (SUCCEEDED(adapter->GetDesc(&adaptDesc)))
//            {
//                const bool isPerfHUD = wcscmp(adaptDesc.Description, L"NVIDIA PerfHUD") == 0;
//                if (nAdapter == 0 || isPerfHUD)
//                {
//                    selectedAdapter = adapter;
//                }
//
//                if (isPerfHUD)
//                {
//                    driverType = D3D_DRIVER_TYPE_REFERENCE;
//                }
//            }
//        }
//        ++nAdapter;
//    }
//#endif

    D3D_FEATURE_LEVEL supportedFeatureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    CLuint supportedFeatureLevelCount = sizeof(supportedFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);

    HRESULT result = D3D11CreateDeviceAndSwapChain( selectedAdapter,
                                                    driverType,
                                                    0,
                                                    createDeviceFlags,
                                                    supportedFeatureLevels,
                                                    supportedFeatureLevelCount,
                                                    D3D11_SDK_VERSION, 
                                                    &swapDesc,
                                                    &m_swapChain,
                                                    &m_device,
                                                    &m_featureLevel,
                                                    &m_deviceContext);

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

    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, 0);

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
    m_deviceContext->RSSetViewports(1, &m_viewPort);
}

void Cloud::Renderer::RenderCore::Present()
{
    m_swapChain->Present(0, 0);

    float clearColour[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColour);
}
