#include "stdafx.h"
#include "GameApplication.h"

#include "renderer/RenderCore.h"
#include "renderer/Settings.h"

#define GAME_WINDOW_STYLE (WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME |  WS_MINIMIZEBOX |  WS_MAXIMIZEBOX)

RPG::Application* RPG::Application::s_instance = 0;

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message) 
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

RPG::Application::Application()
    : m_exitFlag(false)
{
}

RPG::Application::~Application()
{
}

CLbool RPG::Application::Create()
{
    CL_ASSERT(s_instance == 0, "DebugLog already created. Can't re-create!");

    if (s_instance == 0)
    {
        s_instance = new Application();
        return s_instance != 0;
    }

    return false;
}

void RPG::Application::Destroy()
{
    CL_ASSERT(s_instance != 0, "DebugLog not created yet. Can't destroy!");

    if (s_instance != 0)
    {
        SAFE_DELETE(s_instance);
    }
}

CLbool RPG::Application::Initialise()
{
    if (!Cloud::Debug::DebugLog::Create()) { return false; }
    CL_TRACE_CHANNEL("INIT", "DebugLog initialised!");

    if (!Cloud::Renderer::Settings::Create()) { return false; }
    CL_TRACE_CHANNEL("INIT", "Settings initialised!");

    if (!InitialiseWindow()) { return false; }
    CL_TRACE_CHANNEL("INIT", "Window initialised!");

    Cloud::Renderer::RenderCore::Settings renderCoreSettings;
    renderCoreSettings.m_hInstance = m_appInfo.m_hInstance;
    renderCoreSettings.m_cmdShow = m_appInfo.m_cmdShow;
    renderCoreSettings.m_hWnd = m_appInfo.m_hWnd;
    if (!Cloud::Renderer::RenderCore::Create(renderCoreSettings)) { return false; }
    CL_TRACE_CHANNEL("INIT", "RenderCore initialised!");

    if (!m_renderer.Initialise()) { return false; }
    CL_TRACE_CHANNEL("INIT", "Renderer initialised!");

    Cloud::Input::InputManager::Settings inputManagerSettings;
    inputManagerSettings.m_hInstance = m_appInfo.m_hInstance;
    inputManagerSettings.m_hWnd = m_appInfo.m_hWnd;
    if (!m_inputManager.Initialise(inputManagerSettings)) { return false; }
    CL_TRACE_CHANNEL("INIT", "InputManager initialised!");

    if (!m_uiManager.Initialise(m_renderer)) { return false; }
    CL_TRACE_CHANNEL("INIT", "UIManager initialised!");

    return true;
}

void RPG::Application::Shutdown()
{
    m_uiManager.Shutdown();
    m_inputManager.Shutdown();
    m_renderer.Shutdown();

    Cloud::Renderer::RenderCore::Destroy();
    Cloud::Renderer::Settings::Destroy();
    Cloud::Debug::DebugLog::Destroy();
}

void RPG::Application::Run()
{
    MSG msg = {0};
    while (msg.message != WM_QUIT && !m_exitFlag)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Update();
            Render();
        }
    }
}

void RPG::Application::Exit()
{
    m_exitFlag = true;
}

CLbool RPG::Application::InitialiseWindow()
{
    if (!CreateWindowsWindowClass()) return false;
    if (!CreateWindowsWindow()) return false;

    auto result = ShowWindow(m_appInfo.m_hWnd, m_appInfo.m_cmdShow);
    if (result)
    {
        CL_ASSERT_MSG("Failed to create window!");
        return false;
    }

    return true;
}

CLbool RPG::Application::CreateWindowsWindowClass()
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = m_appInfo.m_hInstance;
    wcex.hIcon          = 0;
    wcex.hCursor        = LoadCursor(0, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = "GameWindowClass";
    wcex.hIconSm        = 0;
    if (!RegisterClassEx(&wcex))
    {
        CL_ASSERT_MSG("Failed to create window class!");
        CL_TRACE_CHANNEL("INIT", "Failed to create window class!");
        return false;
    }

    return true;
}

CLbool RPG::Application::CreateWindowsWindow()
{
    RECT rc = {0,
        0,
        Cloud::Renderer::Settings::Instance().GetRoot()["Resolution"]["Width"].asInt(),
        Cloud::Renderer::Settings::Instance().GetRoot()["Resolution"]["Height"].asInt()};

    AdjustWindowRect(&rc, GAME_WINDOW_STYLE, false);

    m_appInfo.m_hWnd = CreateWindow("GameWindowClass",
                                    "Renderer",
                                    GAME_WINDOW_STYLE,
                                    CW_USEDEFAULT,
                                    CW_USEDEFAULT,
                                    rc.right - rc.left,
                                    rc.bottom - rc.top,
                                    0,
                                    0,
                                    m_appInfo.m_hInstance,
                                    0);

    if (!m_appInfo.m_hWnd)
    {
        CL_ASSERT_MSG("Failed to create window!");
        CL_TRACE_CHANNEL("INIT", "Failed to create window!");
        return false;
    }

    return true;
}

void RPG::Application::Update()
{
    m_inputManager.Update();
    m_game.Update();
    m_uiManager.Update();

    if (m_inputManager.GetKeyPressed(Cloud::Input::Keyboard::Escape))
    {
        Exit();
    }
}

void RPG::Application::Render()
{
    m_renderer.Render();
}