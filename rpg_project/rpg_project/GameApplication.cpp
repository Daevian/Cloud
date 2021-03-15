#include "stdafx.h"
#include "GameApplication.h"

#include "renderer/RenderCore.h"
#include "renderer/Settings.h"
#include "renderer/ImguiRenderer.h"

#define GAME_WINDOW_STYLE (WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME |  WS_MINIMIZEBOX |  WS_MAXIMIZEBOX | WS_SYSMENU)

RPG::Application* RPG::Application::s_instance = 0;

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    if (Cloud::Renderer::ImguiRenderer::WndProcHandler(hWnd, message, wParam, lParam))
    {
        return true;
    }

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
    , m_frameCount(0)
    , m_debugCameraActive(true)
    , m_cameraPosition(0.0f, 0.0f, -10.0f, 1.0f)
    , m_cameraRotation(0.0f, 0.0f, 0.0f, 0.0f)
{
}

RPG::Application::~Application()
{
}

bool RPG::Application::Create()
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

bool RPG::Application::Initialise()
{
    if (!Cloud::Debug::DebugLog::Create()) { return false; }
    CL_TRACE_CHANNEL("INIT", "DebugLog initialised!");

    if (!Cloud::CoreThreadPools::Create()) { return false; }
    CL_TRACE_CHANNEL("INIT", "CoreThreadPools initialised!");

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

    m_time.Initialise();

    m_game.Initialise();

    return true;
}

void RPG::Application::Shutdown()
{
    m_uiManager.Shutdown();
    m_inputManager.Shutdown();
    m_renderer.Shutdown();

    Cloud::Renderer::RenderCore::Destroy();
    Cloud::Renderer::Settings::Destroy();
    Cloud::CoreThreadPools::Destroy();
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

            ++m_frameCount;
            const float fps = static_cast<float>(m_frameCount) / static_cast<float>(m_time.GetTotalTime());
            Cloud::StringStream windowText;
            windowText << "FPS: " << fps;
            SetWindowTextA(m_appInfo.m_hWnd, windowText.str().c_str());
        }
    }
}

void RPG::Application::Exit()
{
    m_exitFlag = true;
}

bool RPG::Application::InitialiseWindow()
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

bool RPG::Application::CreateWindowsWindowClass()
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
    wcex.lpszClassName  = L"GameWindowClass";
    wcex.hIconSm        = 0;
    if (!RegisterClassEx(&wcex))
    {
        CL_ASSERT_MSG("Failed to create window class!");
        CL_TRACE_CHANNEL("INIT", "Failed to create window class!");
        return false;
    }

    return true;
}

bool RPG::Application::CreateWindowsWindow()
{
    RECT rc = {0,
        0,
        Cloud::Renderer::Settings::Instance().GetRoot()["Resolution"]["Width"].asInt(),
        Cloud::Renderer::Settings::Instance().GetRoot()["Resolution"]["Height"].asInt()};

    AdjustWindowRect(&rc, GAME_WINDOW_STYLE, false);

    m_appInfo.m_hWnd = CreateWindow(L"GameWindowClass",
                                    L"Renderer",
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
    m_time.Update();
    m_inputManager.Update();
    UpdateDebugCamera((float)m_time.GetTimeStep());
    m_game.Update((float)m_time.GetTimeStep());
    m_uiManager.Update();
    m_renderer.Update(m_time.GetTotalTime(), m_time.GetTimeStep());

    if (m_inputManager.GetKeyPressed(Cloud::Input::Key::Grave))
    {
        Cloud::Renderer::Renderer::s_showDebugMenu = !Cloud::Renderer::Renderer::s_showDebugMenu;
    }

    bool enableInputDebug = true;
    if (enableInputDebug)
    {
        DrawInputDebug();
    }

    if (m_inputManager.GetKeyPressed(Cloud::Input::Key::Escape))
    {
        Exit();
    }
}

void RPG::Application::Render()
{
    m_game.Render();
    m_renderer.Render();
}

void RPG::Application::UpdateDebugCamera(float timeStep)
{
    if (m_inputManager.GetPadButtonPressed(0, Cloud::Input::PadButton::B))
    {
        m_debugCameraActive = !m_debugCameraActive;
        if (m_debugCameraActive)
        {
            m_cameraPosition = m_renderer.GetCamera().GetCameraTransform().GetTranslation();
        }
    }

    if (!m_debugCameraActive)
        return;

    float cameraSpeed = 10.f;
    const float rotationSpeed = 1.5f;
    
    float4 cameraMovement(0.0f);

    {
        cameraSpeed *= m_inputManager.GetPadButtonDown(0, Cloud::Input::PadButton::A) ? 3.0f : 1.0f;

        auto leftStick    = m_inputManager.GetPadLeftStick(0);
        auto rightStick   = m_inputManager.GetPadRightStick(0);
        auto leftTrigger  = m_inputManager.GetPadLeftTrigger(0);
        auto rightTrigger = m_inputManager.GetPadRightTrigger(0);

        cameraMovement = float4(leftStick.x, -leftTrigger + rightTrigger, leftStick.y, 0.0f);
        m_cameraRotation += float4(-rightStick.y, rightStick.x, 0.0f, 0.0f) * rotationSpeed * timeStep;
    }

    if (m_inputManager.GetKeyDown(Cloud::Input::Key::Left))
    {
        m_cameraRotation += float4(0.0f, -1.0f, 0.0f, 0.0f) * rotationSpeed * timeStep;
    }

    if (m_inputManager.GetKeyDown(Cloud::Input::Key::Right))
    {
        m_cameraRotation += float4(0.0f, 1.0f, 0.0f, 0.0f) * rotationSpeed * timeStep;
    }

    if (m_inputManager.GetKeyDown(Cloud::Input::Key::Up))
    {
        m_cameraRotation += float4(-1.0f, 0.0f, 0.0f, 0.0f) * rotationSpeed * timeStep;
    }

    if (m_inputManager.GetKeyDown(Cloud::Input::Key::Down))
    {
        m_cameraRotation += float4(1.0f, 0.0f, 0.0f, 0.0f) * rotationSpeed * timeStep;
    }

    if (m_inputManager.GetKeyDown(Cloud::Input::Key::A))
    {
        cameraMovement = float4(-1.0f, 0.0f, 0.0f, 0.0f);
    }

    if (m_inputManager.GetKeyDown(Cloud::Input::Key::D))
    {
        cameraMovement = float4(1.0f, 0.0f, 0.0f, 0.0f);
    }

    if (m_inputManager.GetKeyDown(Cloud::Input::Key::W))
    {
        cameraMovement = float4(0.0f, 0.0f, 1.0f, 0.0f);
    }

    if (m_inputManager.GetKeyDown(Cloud::Input::Key::S))
    {
        cameraMovement = float4(0.0f, 0.0f, -1.0f, 0.0f);
    }

    if (m_inputManager.GetKeyDown(Cloud::Input::Key::Space))
    {
        cameraMovement = float4(0.0f, 1.0f, 0.0f, 0.0f);
    }

    if (m_inputManager.GetKeyDown(Cloud::Input::Key::LeftCtrl))
    {
        cameraMovement = float4(0.0f, -1.0f, 0.0f, 0.0f);
    }

    ClMatrix4 rotationMatrix = ClMatrix4::Rotation(m_cameraRotation);
    
    m_cameraPosition += rotationMatrix * cameraMovement * cameraSpeed * timeStep;

    ClMatrix4 cameraMatrix = ClMatrix4::Identity();
    cameraMatrix *= ClMatrix4::Rotation(m_cameraRotation);
    cameraMatrix.SetCol3(m_cameraPosition);

    auto& camera = m_renderer.GetCamera();
    camera.SetCameraTransform(cameraMatrix);
}

void RPG::Application::DrawInputDebug()
{
    auto& settings = Cloud::Renderer::Settings::Instance().GetRoot();
    const float width = (float)settings["Resolution"]["Width"].asInt();
    const float height = (float)settings["Resolution"]["Height"].asInt();
    const float ratio = width / height;

    auto& debugRenderer = m_renderer.GetDebugRendererForGame();

    {
        auto leftStick = m_inputManager.GetPadLeftStick(0);
        auto rightStick = m_inputManager.GetPadRightStick(0);

        const float2 centerLeft(-0.5f, -0.5f);
        const float2 centerRight(0.5f, -0.5f);
        const float2 scale(0.2f, 0.2f * ratio);
        debugRenderer.AddLine2D(centerLeft,  centerLeft  + leftStick  * scale, FLOAT4_RED(1.0f));
        debugRenderer.AddLine2D(centerRight, centerRight + rightStick * scale, FLOAT4_RED(1.0f));
    }

    {
        auto leftTrigger = 0.5f;// m_inputManager.GetPadLeftTrigger(0);
        auto rightTrigger = 1.0f;// m_inputManager.GetPadRightTrigger(0);

        const float2 centerLeft(-0.85f, 0.0f);
        const float2 centerRight(0.85f, 0.0f);
        const float2 scale(0.1f, 0.8f);
        debugRenderer.AddBar(centerLeft,  scale, FLOAT4_RED(0.7f), leftTrigger);
        debugRenderer.AddBar(centerRight, scale, FLOAT4_RED(0.7f), rightTrigger);
    }
}