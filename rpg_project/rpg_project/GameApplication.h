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
        int m_cmdShow;
        HWND m_hWnd;
    };

    class Application
    {
    public:
        static bool Create();
        static void Destroy();

        static Application& Instance() { return *s_instance; }

        bool Initialise();
        void Shutdown();

        void Run();
        void Exit();

        WindowsApplicationInfo m_appInfo;

        Cloud::Renderer::Renderer&  GetRenderer()   { return m_renderer; }
        UIManager&                  GetUIManager()  { return m_uiManager; }
        Cloud::Input::InputManager& GetInput()      { return m_inputManager; }

        bool IsDebugCameraActive() const          { return m_debugCameraActive; }

    private:
        Application();
        ~Application();

        bool InitialiseWindow();
        bool CreateWindowsWindowClass();
        bool CreateWindowsWindow();

        void Update();
        void Render();

        void UpdateDebugCamera(float timeStep);
        void DrawInputDebug();

        static Application* s_instance;

        Cloud::Utils::Timer m_time;
        Cloud::Renderer::Renderer m_renderer;
        Cloud::Input::InputManager m_inputManager;
        UIManager m_uiManager;
        Game m_game;

        bool m_exitFlag;
        uint m_frameCount;

        bool m_debugCameraActive;
        float4 m_cameraPosition;
        float4 m_cameraRotation;
    };
}

#endif // RPG_APPLICATION_HEADER