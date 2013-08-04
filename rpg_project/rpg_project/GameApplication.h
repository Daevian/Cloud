#ifndef RPG_APPLICATION_HEADER
#define RPG_APPLICATION_HEADER

#include "UIManager.h"
#include "renderer/Renderer.h"
#include "input/InputManager.h"
#include "Game.h"

namespace RPG
{
    struct WindowsApplicationInfo
    {
        HINSTANCE m_hInstance;
        HINSTANCE m_prevHInstance;
        LPWSTR m_cmdLine;
        CLint m_cmdShow;
        HWND m_hWnd;
    };

    class Application
    {
    public:
        static CLbool Create();
        static void Destroy();

        static Application& Instance() { return *s_instance; }

        CLbool Initialise();
        void Shutdown();

        void Run();
        void Exit();

        WindowsApplicationInfo m_appInfo;

        Cloud::Renderer::Renderer&  GetRenderer()   { return m_renderer; }
        UIManager&                  GetUIManager()  { return m_uiManager; }

    private:
        Application();
        ~Application();

        CLbool InitialiseWindow();
        CLbool CreateWindowsWindowClass();
        CLbool CreateWindowsWindow();

        void Update();
        void Render();

        static Application* s_instance;

        Cloud::Utils::Timer m_time;
        Cloud::Renderer::Renderer m_renderer;
        Cloud::Input::InputManager m_inputManager;
        UIManager m_uiManager;
        Game m_game;

        CLbool m_exitFlag;
        CLuint m_frameCount;
    };
}

#endif // RPG_APPLICATION_HEADER