#ifndef CLOUD_RENDERER_RENDERCORE_HEADER
#define CLOUD_RENDERER_RENDERCORE_HEADER

#include "RenderRenderingDevice.h"

#include "TextureContainer.h"
#include "ShaderEffectContainer.h"
#include "GfxConstantBuffer.h"
#include "GfxTexture.h"

namespace Cloud
{
    namespace Renderer
    {
        class GfxTexture;
        struct GfxTextureDesc;

        struct PerSceneConstBuffer
        {
            Math::Matrix4 view;
            Math::Matrix4 projection;
        };

        struct PerModelConstBuffer
        {
            Math::Matrix4 model;
        };

        class RenderCore
        {
        public:
            struct Settings
            {
                HINSTANCE m_hInstance;
                CLint m_cmdShow;
                HWND m_hWnd;
            };

            static CLbool Create(const Settings& settings);
            static void Destroy();

            static RenderCore& Instance() { return *s_instance; }

            void Present();

            ID3D11Device* GetDevice() { return m_device; }
            ID3D11DeviceContext* GetContext() { return m_context; }
            RenderingDevice& GetRenderingDevice() { return m_renderingDevice; }

            TextureContainer& GetTextureContainer() { return m_textureContainer; }
            ShaderEffectContainer& GetEffectContainer() { return m_effectContainer; }

            PerSceneConstBuffer& GetPerSceneConstData() { return m_perSceneConstData; }
            PerModelConstBuffer& GetPerModelConstData() { return m_perModelConstData; }

            void GpuUpdatePerSceneConstBuffer();
            void GpuUpdatePerModelConstBuffer();

            GfxTexture* Create(const GfxTextureDesc& desc);
            void Destroy(GfxTexture* texture);

        private:
            RenderCore();
            ~RenderCore();

            CLbool Initialise(const Settings& settings);
            void Shutdown();

            CLbool InitSwapChain();
            CLbool InitBackBuffer();
            CLbool InitDepthBuffer();
            CLbool InitConstantBuffers();
            void InitViewPort();

            CLuint GetMSAAQuality(CLuint samples, DXGI_FORMAT format);

            static RenderCore* s_instance;

            TextureContainer m_textureContainer;
            ShaderEffectContainer m_effectContainer;
            GfxTextureFactory m_gfxTextureFactory;
            
            ID3D11Device* m_device;
            ID3D11DeviceContext* m_context;
            IDXGISwapChain* m_swapChain;
            ID3D11RenderTargetView* m_renderTargetView;
            ID3D11DepthStencilView* m_depthStencilView;
            ID3D11Texture2D* m_depthStencilBuffer;

            D3D_FEATURE_LEVEL m_featureLevel;
            
            D3D11_VIEWPORT m_viewPort;

            GfxConstantBuffer m_perSceneConstBuffer;
            GfxConstantBuffer m_perModelConstBuffer;
            PerSceneConstBuffer m_perSceneConstData;
            PerModelConstBuffer m_perModelConstData;

            RenderingDevice m_renderingDevice;
            Settings m_settings;
        };

        typedef RenderCore GfxCore;
    }
}

#endif // CLOUD_RENDERER_RENDERCORE_HEADER