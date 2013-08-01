#ifndef CLOUD_RENDERER_RENDERCORE_HEADER
#define CLOUD_RENDERER_RENDERCORE_HEADER

#include "RenderRenderingDevice.h"

#include "TextureContainer.h"
#include "ShaderEffectContainer.h"

namespace Cloud
{
    namespace Renderer
    {
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
            ID3D11DeviceContext* GetDeviceContext() { return m_deviceContext; }
            RenderingDevice& GetRenderingDevice() { return m_renderingDevice; }

            TextureContainer& GetTextureContainer() { return m_textureContainer; }
            ShaderEffectContainer& GetEffectContainer() { return m_effectContainer; }

        private:
            RenderCore();
            ~RenderCore();

            CLbool Initialise(const Settings& settings);
            void Shutdown();

            CLbool InitSwapChain();
            CLbool InitBackBuffer();
            void InitViewPort();

            static RenderCore* s_instance;

            TextureContainer m_textureContainer;
            ShaderEffectContainer m_effectContainer;
            
            ID3D11Device* m_device;
            ID3D11DeviceContext* m_deviceContext;
            IDXGISwapChain* m_swapChain;
            ID3D11RenderTargetView* m_renderTargetView;
            D3D_FEATURE_LEVEL m_featureLevel;
            
            D3D11_VIEWPORT m_viewPort;

            RenderingDevice m_renderingDevice;
            Settings m_settings;
        };
    }
}

#endif // CLOUD_RENDERER_RENDERCORE_HEADER