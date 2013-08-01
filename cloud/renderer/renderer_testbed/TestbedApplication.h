#ifndef RPG_GAME_APPLICATION_HEADER
#define RPG_GAME_APPLICATION_HEADER

#include "../renderer/Renderer.h"

namespace RPG
{
    class WindowsApplicationInfo
    {
    public:
        HINSTANCE m_hInstance;
        HINSTANCE m_prevHInstance;
        LPWSTR m_cmdLine;
        CLint m_cmdShow;
    };

    class TestbedApplication
    {
    public:
        TestbedApplication();

        CLbool Initialise();
        void Shutdown();

        void Run();

        WindowsApplicationInfo m_appInfo;

    private:
        CLbool CreateSingletons();
        void DestroySingletons();

        void CreateSprites();

        void Update();
        void Render();

        Cloud::Renderer::Renderer m_renderer;
        Cloud::Renderer::Sprite* m_sprite;
        Cloud::Renderer::Sprite* m_sprite2;
        
    };
}

#endif // RPG_GAME_APPLICATION_HEADER