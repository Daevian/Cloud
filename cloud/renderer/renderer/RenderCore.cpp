#include "stdafx.h"
#include "RenderCore.h"

#include "Settings.h"
#include "GfxTexture.h"

std::unique_ptr<Cloud::Renderer::RenderCore, Cloud::Renderer::RenderCore::Deleter> Cloud::Renderer::RenderCore::s_instance = nullptr;

Cloud::Renderer::RenderCore::RenderCore()
#ifdef USE_DIRECTX12
    : m_featureLevel(D3D_FEATURE_LEVEL_12_1)
#else
    : m_featureLevel(D3D_FEATURE_LEVEL_11_1)
#endif
{
    ClMemZero(&m_settings, sizeof(m_settings));
}

Cloud::Renderer::RenderCore::~RenderCore()
{
}

bool Cloud::Renderer::RenderCore::Create(const Settings& settings)
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

bool Cloud::Renderer::RenderCore::Initialise(const Settings& settings)
{
    m_settings = settings;

#ifdef _DEBUG
    if (!EnableDebugLayer()) return false;
#endif

    UINT dxgiFactoryFlags = 0;
#ifdef _DEBUG
    dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

    if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory))))
    {
        CL_ASSERT_MSG("Failed to create dxgi factory!");
        return false;
    }

    m_hardwareAdapter = GetHardwareAdapter();

    if (FAILED(D3D12CreateDevice(m_hardwareAdapter.Get(), m_featureLevel, IID_PPV_ARGS(&m_device))))
    {
        CL_ASSERT_MSG("Failed to create device!");
        return false;
    }

    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        if (FAILED(m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue))))
        {
            CL_ASSERT_MSG("Failed to create command queue!");
            return false;
        }
    }

    if (!InitSwapChain()) return false;
    //if (!InitBackBuffer()) return false;
    if (!InitDepthBuffer()) return false;
    if (!InitConstantBuffers()) return false;
    InitViewPort();

    // m_frameBeginCl
    {
        if (FAILED(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_frameBeginCl))))
        {
            CL_ASSERT_MSG("Failed to create command list!");
            return false;
        }

        // The main loop expects it to be closed, so close it now by default.
        if (FAILED(m_frameBeginCl->Close()))
        {
            CL_ASSERT_MSG("Failed to close command list!");
            return false;
        }
    }

    // m_frameEndCl
    {
        if (FAILED(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_frameEndCl))))
        {
            CL_ASSERT_MSG("Failed to create command list!");
            return false;
        }

        // The main loop expects it to be closed, so close it now by default.
        if (FAILED(m_frameEndCl->Close()))
        {
            CL_ASSERT_MSG("Failed to close command list!");
            return false;
        }
    }

    // m_commandList
    {
        for (auto&& commandList : m_commandLists)
        {
            if (FAILED(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList))))
            {
                CL_ASSERT_MSG("Failed to create command list!");
                return false;
            }

            // The main loop expects it to be closed, so close it now by default.
            if (FAILED(commandList->Close()))
            {
                CL_ASSERT_MSG("Failed to close command list!");
                return false;
            }
        }
        
    }

    if (FAILED(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence))))
    {
        CL_ASSERT_MSG("Failed to create fence!");
        return false;
    }

    m_fenceValue = 1;

    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!m_fenceEvent)
    {
        if (FAILED(HRESULT_FROM_WIN32(GetLastError())))
        {
            CL_ASSERT_MSG("Failed to create fence event!");
            return false;
        }
    }
    

    // Create root signature.
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
        {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        // init ranges
        /*std::array<CD3DX12_DESCRIPTOR_RANGE1, 2> ranges;
        std::array<CD3DX12_ROOT_PARAMETER1, 1> rootParameters;*/
        std::array<CD3DX12_ROOT_PARAMETER1, 5> rootParameters;
        std::array<CD3DX12_DESCRIPTOR_RANGE1, 1> ranges;

        //ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, c_cbvDescCount, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        //ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, c_srvDescCount, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        //rootParameters[0].InitAsDescriptorTable(2, ranges.data(), D3D12_SHADER_VISIBILITY_ALL);
        //uint rootParam = 0;
        uint cbufferIndex = 0;
        rootParameters[0].InitAsConstantBufferView(cbufferIndex++); // per scene
        rootParameters[1].InitAsConstantBufferView(cbufferIndex++); // per model
        rootParameters[2].InitAsConstantBufferView(cbufferIndex++); // per material

        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, c_srvDescCount, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        rootParameters[3].InitAsDescriptorTable(1, ranges.data(), D3D12_SHADER_VISIBILITY_PIXEL);

        rootParameters[4].InitAsConstantBufferView(cbufferIndex++); // lighting

        // init samplers
        D3D12_STATIC_SAMPLER_DESC sampler = {};
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 0;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = 0;
        sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        
        // init root descriptor
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
        desc.Init_1_1(static_cast<uint>(rootParameters.size()), rootParameters.data(), 1, &sampler, rootSignatureFlags);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        if (FAILED(D3DX12SerializeVersionedRootSignature(&desc, featureData.HighestVersion, &signature, &error)))
        {
            CL_ASSERT_MSG("Failed to create fence event!");
            return false;
        }

        if (FAILED(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature))))
        {
            CL_ASSERT_MSG("Failed to create fence event!");
            return false;
        }
    }

    CL_TRACE_CHANNEL("INIT", "[RenderCore] Initialised!");
    return true;
}

bool Cloud::Renderer::RenderCore::EnableDebugLayer()
{
    ComPtr<ID3D12Debug> debugController;
    if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        CL_ASSERT_MSG("Failed to enable debug layer!");
        return false;
    }

    debugController->EnableDebugLayer();
    return true;
}

Cloud::ComPtr<IDXGIAdapter1> Cloud::Renderer::RenderCore::GetHardwareAdapter()
{
    ComPtr<IDXGIAdapter1> adapter;

    for (UINT adapterIndex = 0; m_factory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND; ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            continue;
        }

        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), m_featureLevel, _uuidof(ID3D12Device), nullptr)))
        {
            return adapter.Detach();
        }
    }

    CL_ASSERT_MSG("couldn't find a hardware adapter with required feature level!");
    return nullptr;
}

void Cloud::Renderer::RenderCore::Shutdown()
{
#ifdef USE_DIRECTX12
    // Ensure that the GPU is no longer referencing resources that are about to be
    // cleaned up by the destructor.
    WaitForPreviousFrame();

    CloseHandle(m_fenceEvent);
#else
    if (m_context) m_context->ClearState();

    m_perSceneConstBuffer.Uninitialise();
    m_perModelConstBuffer.Uninitialise();
    m_depthStencil = nullptr;
    m_backbuffer = nullptr;
    m_swapChain = nullptr;
    m_context = nullptr;
    m_device = nullptr;
#endif

    CL_TRACE_CHANNEL("INIT", "[RenderCore] Shut down!");
}

bool Cloud::Renderer::RenderCore::InitSwapChain()
{
#ifdef USE_DIRECTX12

    auto& settings = Cloud::Renderer::Settings::Instance().GetRoot();

    {
        DXGI_SWAP_CHAIN_DESC1 desc = {};
        desc.BufferCount = c_frameBufferCount;
        desc.Width = settings["Resolution"]["Width"].asInt();
        desc.Height = settings["Resolution"]["Height"].asInt();
        //desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;// | DXGI_USAGE_SHADER_INPUT;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.SampleDesc.Count = settings["Graphics"]["MSAA"].asInt();

        ComPtr<IDXGISwapChain1> swapChain;
        if (FAILED(m_factory->CreateSwapChainForHwnd(m_commandQueue.Get(), m_settings.m_hWnd, &desc, nullptr, nullptr, &swapChain)))
        {
            CL_ASSERT_MSG("Failed to create swap chain!");
            return false;
        }

        if (FAILED(m_factory->MakeWindowAssociation(m_settings.m_hWnd, DXGI_MWA_NO_ALT_ENTER)))
        {
            CL_ASSERT_MSG("couldn't associate window with device!");
            return false;
        }

        if (FAILED(swapChain.As(&m_swapChain)))
        {
            return false;
        }

        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = c_frameBufferCount;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        if (FAILED(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_rtvHeap))))
        {
            CL_ASSERT_MSG("couldn't create RTV desc heap!");
            return false;
        }

        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = 1;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        if (FAILED(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_dsvHeap))))
        {
            CL_ASSERT_MSG("couldn't create DSVdesc heap!");
            return false;
        }

        m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = c_cbvDescCount + c_srvDescCount;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (FAILED(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_cbvHeap))))
        {
            CL_ASSERT_MSG("couldn't create CBV desc heap!");
            return false;
        }
    }

    /*CD3DX12_CPU_DESCRIPTOR_HANDLE srv = {};
    static const auto c_srvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    srv.InitOffsetted(RenderCore::Instance().GetCbvHeap()->GetCPUDescriptorHandleForHeapStart(), 1, c_srvDescriptorSize);

    D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
    / *viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    viewDesc.Format = DXGI_FORMAT_UNKNOWN;
    viewDesc.ViewDimension = D3D12_SRV_DIMENSION_UNKNOWN;
    viewDesc.Texture2D.MostDetailedMip = 0;
    viewDesc.Texture2D.PlaneSlice = 0;
    viewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    viewDesc.Texture2D.MipLevels = 0;* /

    m_device->CreateShaderResourceView(nullptr, &viewDesc, srv);*/

    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

        for (UINT i = 0; i < c_frameBufferCount; i++)
        {
            if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]))))
            {
                CL_ASSERT_MSG("couldn't get swap chain buffer!");
                return false;
            }

            m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);
        }
    }    

    if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator))))
    {
        CL_ASSERT_MSG("couldn't create command allocator!");
        return false;
    }

    return true;

#else

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

    uint supportedFeatureLevelCount = sizeof(supportedFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);
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
#endif // USE_DIRECTX12
}

bool Cloud::Renderer::RenderCore::InitBackBuffer()
{
#ifdef USE_DIRECTX12

    return false;
#else
    m_backbuffer = m_gfxTextureFactory.CreateFromBackbuffer();
    if (!m_backbuffer)
    {
        CL_ASSERT_MSG("Failed to create back buffer!");
        CL_TRACE_CHANNEL("INIT", "[RenderCore] Failed to create back buffer!");
        return false;
    }

    return true;
#endif // USE_DIRECTX12
}

bool Cloud::Renderer::RenderCore::InitDepthBuffer()
{
    auto& settings = Cloud::Renderer::Settings::Instance().GetRoot();

    GfxTextureDesc desc = {};

#ifdef USE_DIRECTX12
    desc.name = "depthstencil";
    desc.dim = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.width = settings["Resolution"]["Width"].asInt();
    desc.height = settings["Resolution"]["Height"].asInt();
    desc.mipCount = 1;
    desc.arraySize = 1;
    desc.format = (DXGI_FORMAT)GfxFormat::R32_TYPELESS;
    desc.sampleDesc.Count = settings["Graphics"]["MSAA"].asInt();
    desc.sampleDesc.Quality = 0;
    desc.heapType = D3D12_HEAP_TYPE_DEFAULT;
    desc.initialState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    desc.flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
    desc.clearValue.Format = DXGI_FORMAT_D32_FLOAT;
    desc.clearValue.DepthStencil.Depth = 1.0f;
#else
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
#endif // USE_DIRECTX12

    m_depthStencil = Create(desc);
    if (!m_depthStencil)
    {
        CL_ASSERT_MSG("Failed to create depth stencil!");
        CL_TRACE_CHANNEL("INIT", "[RenderCore] Failed to depth stencil!");
        return false;
    }

    return true;
}

bool Cloud::Renderer::RenderCore::InitConstantBuffers()
{
    m_perSceneConstBuffer.SetData(&m_perSceneConstData, sizeof(PerSceneConstBuffer));
    if (!m_perSceneConstBuffer.Initialise(128))
        return false;
    
    m_perModelConstBuffer.SetData(&m_perModelConstData, sizeof(PerModelConstBuffer));
    if (!m_perModelConstBuffer.Initialise(128))
        return false;

    return true;
}

void Cloud::Renderer::RenderCore::InitViewPort()
{
    m_viewPort.Width = static_cast<float>(Cloud::Renderer::Settings::Instance().GetRoot()["Resolution"]["Width"].asDouble());
    m_viewPort.Height = static_cast<float>(Cloud::Renderer::Settings::Instance().GetRoot()["Resolution"]["Height"].asDouble());
    m_viewPort.MinDepth = 0.0f;
    m_viewPort.MaxDepth = 1.0f;
    m_viewPort.TopLeftX = 0;
    m_viewPort.TopLeftY = 0;

#ifdef USE_DIRECTX12
    m_scissorRect.left = 0;
    m_scissorRect.top = 0;
    m_scissorRect.right = Cloud::Renderer::Settings::Instance().GetRoot()["Resolution"]["Width"].asInt();
    m_scissorRect.bottom = Cloud::Renderer::Settings::Instance().GetRoot()["Resolution"]["Height"].asInt();
#else
    m_context->RSSetViewports(1, &m_viewPort);
#endif
}

CD3DX12_CPU_DESCRIPTOR_HANDLE Cloud::Renderer::RenderCore::GetCurrentBackBuffer()
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
}

void Cloud::Renderer::RenderCore::GpuUpdatePerSceneConstBuffer()
{
#ifdef USE_DIRECTX12
    m_perSceneConstBuffer.GPUUpdateConstantBuffer();
#else
    m_renderingDevice.SetConstantBuffer(&m_perSceneConstBuffer, 0);
    m_perSceneConstBuffer.GPUUpdateConstantBuffer();
#endif
}

void Cloud::Renderer::RenderCore::GpuUpdatePerModelConstBuffer()
{
#ifdef USE_DIRECTX12
    m_perModelConstBuffer.GPUUpdateConstantBuffer();
#else
    m_renderingDevice.SetConstantBuffer(&m_perModelConstBuffer, 1);
    m_perModelConstBuffer.GPUUpdateConstantBuffer();
#endif
}

void Cloud::Renderer::RenderCore::QueueRecordCommandListJob(const RecordCommandListJob& func)
{
    m_recordCommandListJobs.emplace_back(func);
}

void Cloud::Renderer::RenderCore::RecordCommandLists()
{
    // must wait for prev frame to finish before this is called
    if (FAILED(m_commandAllocator->Reset()))
    {
        CL_ASSERT_MSG("Failed to reset command allocator!");
    }

    // Frame begin commands
    {
        if (FAILED(m_frameBeginCl->Reset(m_commandAllocator.Get(), m_pipelineState.Get())))
        {
            CL_ASSERT_MSG("Failed to reset command list!");
        }

        auto presentToRtvBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_frameBeginCl->ResourceBarrier(1, &presentToRtvBarrier);

        auto&& rtvHandle = GetCurrentBackBuffer();
        const std::array<float, 4> clearColor = { 0.0f, 0.2f, 0.4f, 1.0f };
        m_frameBeginCl->ClearRenderTargetView(rtvHandle, clearColor.data(), 0, nullptr);

        if (FAILED(m_frameBeginCl->Close()))
        {
            CL_ASSERT_MSG("Failed to close command list!");
        }
    }

    // jobs
    for (auto&& job : m_recordCommandListJobs)
    {
        auto&& commandList = AllocateCommandList();

        job(commandList);

        if (FAILED(commandList->Close()))
        {
            CL_ASSERT_MSG("Failed to close command list!");
        }
    }
    

    // Frame end commands
    {
        if (FAILED(m_frameEndCl->Reset(m_commandAllocator.Get(), m_pipelineState.Get())))
        {
            CL_ASSERT_MSG("Failed to reset command list!");
        }

        m_frameEndCl->SetGraphicsRootSignature(m_rootSignature.Get());

        auto rtvToPresentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

        m_frameEndCl->ResourceBarrier(1, &rtvToPresentBarrier);

        if (FAILED(m_frameEndCl->Close()))
        {
            CL_ASSERT_MSG("Failed to close command list!");
        }
    }
}

void Cloud::Renderer::RenderCore::Present()
{
#ifdef USE_DIRECTX12

    std::vector<ID3D12CommandList*> commandLists;
    commandLists.emplace_back(m_frameBeginCl.Get());
    for (uint i = 0; i < m_recordedCommandLists; i++)
    {
        commandLists.emplace_back(m_commandLists[i].Get());
    }

    commandLists.emplace_back(m_frameEndCl.Get());

    m_recordedCommandLists = 0;


    m_commandQueue->ExecuteCommandLists(static_cast<uint>(commandLists.size()), commandLists.data());

    const bool enableVSync = Cloud::Renderer::Settings::Instance().GetRoot()["Graphics"]["VSync"].asBool();

    if (FAILED(m_swapChain->Present(enableVSync ? 1 : 0, 0)))
    {
        CL_ASSERT_MSG("Failed to present frame!");
    }

    m_recordCommandListJobs.clear();
    

    WaitForPreviousFrame();

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    m_perModelConstBuffer.Reset();
    m_perSceneConstBuffer.Reset();

#else
    const bool enableVSync = Cloud::Renderer::Settings::Instance().GetRoot()["Graphics"]["VSync"].asBool();
    m_swapChain->Present(enableVSync ? 1 : 0, 0);
#endif
}

void Cloud::Renderer::RenderCore::Flush()
{
    std::vector<ID3D12CommandList*> commandLists;
    for (uint i = 0; i < m_recordedCommandLists; i++)
    {
        commandLists.emplace_back(m_commandLists[i].Get());
    }

    m_recordedCommandLists = 0;

    m_commandQueue->ExecuteCommandLists(static_cast<uint>(commandLists.size()), commandLists.data());

    // wait for gpu idle
    WaitForPreviousFrame();
}

ID3D12GraphicsCommandList* Cloud::Renderer::RenderCore::AllocateCommandList()
{
    auto&& commandList = m_commandLists[m_recordedCommandLists++].Get();

    // can be reset anytime after ExecuteCommandList(), and always has to be reset before recording
    if (FAILED(commandList->Reset(GetCommandAllocator(), nullptr)))
    {
        CL_ASSERT_MSG("Failed to reset command list!");
    }

    return commandList;
}

void Cloud::Renderer::RenderCore::WaitForPreviousFrame()
{
    const UINT64 fence = m_fenceValue;
    if (FAILED(m_commandQueue->Signal(m_fence.Get(), fence)))
    {
        CL_ASSERT_MSG("Failed to signal fence!");
    }

    m_fenceValue++;

    // Wait until the previous frame is finished.
    if (m_fence->GetCompletedValue() < fence)
    {
        if (FAILED(m_fence->SetEventOnCompletion(fence, m_fenceEvent)))
        {
            CL_ASSERT_MSG("Failed to set event on fence completion!");
        }

        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}

uint Cloud::Renderer::RenderCore::GetMSAAQuality(uint /*samples*/, DXGI_FORMAT /*format*/)
{
#ifdef USE_DIRECTX12
    return 0;
#else
    uint quality = 1;
    HRESULT result = m_device->CheckMultisampleQualityLevels(format, samples, &quality);
    if (FAILED(result))
    {
        CL_ASSERT_MSG("Failed to get MSAA quality on format!");
    }

    return quality - 1;
#endif
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

#ifdef USE_DIRECTX12
#else
void Cloud::Renderer::RenderCore::SetDebugObjectName(ID3D11DeviceChild* resource, const t_char* name)
{
    {
#if defined(_DEBUG) || defined(PROFILE)
        resource->SetPrivateData(WKPDID_D3DDebugObjectName, (uint)ClStrlen(name), name);
#else
        CL_UNUSED(resource);
        CL_UNUSED(name);
#endif
    }
}
#endif
