#ifndef CLOUD_RENDERER_RENDERCORE_HEADER
#define CLOUD_RENDERER_RENDERCORE_HEADER

#include "RenderRenderingDevice.h"

#include "TextureContainer.h"
#include "ShaderEffectContainer.h"
#include "GfxConstantBuffer.h"
#include "GfxBuffer.h"
#include "GfxTexture.h"
#include "GfxShader.h"

namespace Cloud
{
    namespace Renderer
    {
        struct PerSceneConstBuffer
        {
            Math::Matrix4 view;
            Math::Matrix4 projection;
            Math::Matrix4 viewProj;
            Math::Matrix4 invView;
        };

        struct PerModelConstBuffer
        {
            Math::Matrix4 model;
        };

        class RenderCore
        {
        public:
            using RecordCommandListJob = std::function<void(ID3D12GraphicsCommandList*)>;

            struct Settings
            {
                HINSTANCE m_hInstance;
                int m_cmdShow;
                HWND m_hWnd;
            };

            static bool Create(const Settings& settings);
            static void Destroy();

            static RenderCore& Instance() { return *s_instance; }

            void QueueRecordCommandListJob(const RecordCommandListJob& func);
            void RecordCommandLists();
            void Present();
            void Flush();

            ID3D12GraphicsCommandList* AllocateCommandList();

            void WaitForPreviousFrame();

            const Settings&         GetSettings() const             { return m_settings; }

#ifdef USE_DIRECTX12
            ID3D12Device*           GetDevice()                     { return m_device.Get(); }
#else
            ID3D11Device*           GetDevice()                     { return m_device.get(); }
            ID3D11DeviceContext*    GetContext()                    { return m_context.get(); }
#endif
            RenderingDevice&        GetRenderingDevice()            { return m_renderingDevice; }
            GfxTexture*             GetBackbuffer()                 { return m_backbuffer.get(); }
            GfxTexture*             GetDepthStencil()               { return m_depthStencil.get(); }

            TextureContainer&       GetTextureContainer()           { return m_textureContainer; }
            ShaderEffectContainer&  GetEffectContainer()            { return m_effectContainer; }

            PerSceneConstBuffer&    GetPerSceneConstData()          { return m_perSceneConstData; }
            PerModelConstBuffer&    GetPerModelConstData()          { return m_perModelConstData; }

            GfxConstantBuffer&      GetPerSceneConstBuffer()        { return m_perSceneConstBuffer; }
            GfxConstantBuffer&      GetPerModelConstBuffer()        { return m_perModelConstBuffer; }

#ifdef USE_DIRECTX12
            ID3D12RootSignature*    GetRootSignature() { return m_rootSignature.Get(); }
            ID3D12CommandAllocator* GetCommandAllocator() { return m_commandAllocator.Get(); }
            ID3D12DescriptorHeap*   GetCbvHeap() { return m_cbvHeap.Get(); }
            ID3D12DescriptorHeap*   GetDsvHeap() { return m_dsvHeap.Get(); }
            //ID3D12DescriptorHeap*   GetSrvHeap() { return m_cbvHeap.Get(); }
            static const uint c_cbvDescCount = 2;
            static const uint c_srvDescCount = 1;
            uint m_cbvHeapIndex = 0;
            uint m_srvHeapIndex = 0;
            uint m_dsvHeapIndex = 0;
            CD3DX12_VIEWPORT&       GetViewPort() { return m_viewPort; }
            CD3DX12_RECT&           GetScissorRect() { return m_scissorRect; }
            CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBuffer();
#else
            IDXGISwapChain*         GetSwapChain()                  { return m_swapChain.get(); }
#endif

            void GpuUpdatePerSceneConstBuffer();
            void GpuUpdatePerModelConstBuffer();

            GfxBuffer::UniquePtr        Create(const GfxBufferDesc& desc);
            GfxTexture::UniquePtr       Create(const GfxTextureDesc& desc);
            GfxComputeShader::UniquePtr Create(const GfxComputerShaderDesc& desc);

#ifdef USE_DIRECTX12
#else
            static void SetDebugObjectName(ID3D11DeviceChild* resource, const t_char* name);
#endif

        private:
            RenderCore();
            ~RenderCore();

            struct Deleter
            {
                void operator()(RenderCore* ptr) const
                {
                    delete ptr;
                }
            };

            bool Initialise(const Settings& settings);

            void Shutdown();

            bool EnableDebugLayer();
            bool InitSwapChain();
            bool InitBackBuffer();
            bool InitDepthBuffer();
            bool InitConstantBuffers();
            void InitViewPort();

            uint GetMSAAQuality(uint samples, DXGI_FORMAT format);

            static std::unique_ptr<RenderCore, Deleter> s_instance;

            TextureContainer m_textureContainer;
            ShaderEffectContainer m_effectContainer;
            GfxBufferFactory m_gfxBufferFactory;
            GfxTextureFactory m_gfxTextureFactory;
            GfxShaderFactory m_gfxShaderFactory;
            
#ifdef USE_DIRECTX12
            static const uint c_frameBufferCount = 2;

            ComPtr<IDXGIFactory4> m_factory;
            ComPtr<IDXGIAdapter1> m_hardwareAdapter;
            ComPtr<ID3D12Device> m_device;
            ComPtr<ID3D12CommandQueue> m_commandQueue;
            ComPtr<IDXGISwapChain3> m_swapChain;
            ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
            ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
            ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
            //ComPtr<ID3D12DescriptorHeap> m_srvHeap;
            ComPtr<ID3D12CommandAllocator> m_commandAllocator;
            std::array<ComPtr<ID3D12Resource>, c_frameBufferCount> m_renderTargets;

            UINT m_rtvDescriptorSize = 0;
            UINT m_dsvDescriptorSize = 0;

            UINT m_frameIndex = 0;

            uint m_recordedCommandLists = 0;
            std::array<ComPtr<ID3D12GraphicsCommandList>, 10> m_commandLists;
            ComPtr<ID3D12GraphicsCommandList> m_frameBeginCl;
            ComPtr<ID3D12GraphicsCommandList> m_frameEndCl;
            ComPtr<ID3D12RootSignature> m_rootSignature;
            ComPtr<ID3D12PipelineState> m_pipelineState;

            ComPtr<ID3D12Fence> m_fence;
            HANDLE m_fenceEvent;
            UINT64 m_fenceValue = 0;

            CD3DX12_VIEWPORT m_viewPort;
            CD3DX12_RECT m_scissorRect;

            std::vector<RecordCommandListJob> m_recordCommandListJobs;

#else
            Dx::UniquePtr<ID3D11Device> m_device;
            Dx::UniquePtr<ID3D11DeviceContext> m_context;
            Dx::UniquePtr<IDXGISwapChain> m_swapChain;
            D3D11_VIEWPORT m_viewPort;
#endif
            GfxTexture::UniquePtr m_backbuffer;
            GfxTexture::UniquePtr m_depthStencil;
            D3D_FEATURE_LEVEL m_featureLevel;
           

            GfxConstantBuffer m_perSceneConstBuffer;
            GfxConstantBuffer m_perModelConstBuffer;
            PerSceneConstBuffer m_perSceneConstData;
            PerModelConstBuffer m_perModelConstData;

            RenderingDevice m_renderingDevice;
            Settings m_settings;
            ComPtr<IDXGIAdapter1> GetHardwareAdapter();
        };

        typedef RenderCore GfxCore;
    }
}

#endif // CLOUD_RENDERER_RENDERCORE_HEADER