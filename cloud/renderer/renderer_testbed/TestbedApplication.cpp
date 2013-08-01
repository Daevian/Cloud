#include "stdafx.h"
#include "TestbedApplication.h"

#include "../../renderer/renderer/RenderCore.h"
#include "../../renderer/renderer/Settings.h"

RPG::TestbedApplication::TestbedApplication()
:m_sprite(0)
,m_sprite2(0)
{
}

CLbool RPG::TestbedApplication::Initialise()
{
    if (!CreateSingletons()) return false;

    if (!m_renderer.Initialise()) return false;

    return true;
}

void RPG::TestbedApplication::Shutdown()
{
    DestroySingletons();
}

CLbool RPG::TestbedApplication::CreateSingletons()
{
    if (!Cloud::Debug::DebugLog::Create()) { return false; }

    if (!Cloud::Renderer::Settings::Create()) { return false; }

    Cloud::Renderer::RenderCoreSettings renderCoreSettings;
    renderCoreSettings.m_hInstance = m_appInfo.m_hInstance;
    renderCoreSettings.m_cmdShow = m_appInfo.m_cmdShow;

    if (!Cloud::Renderer::RenderCore::Create(renderCoreSettings)) { return false; }

    CL_TRACE_CHANNEL("INIT", "All singletons initialised!");
    return true;
}

void RPG::TestbedApplication::DestroySingletons()
{
    Cloud::Renderer::RenderCore::Destroy();
    Cloud::Renderer::Settings::Destroy();
    Cloud::Debug::DebugLog::Destroy();
}

void RPG::TestbedApplication::Run()
{
    MSG msg = {0};
    while (msg.message != WM_QUIT)
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

void RPG::TestbedApplication::Update()
{
    if (m_sprite == 0)
    {
        m_sprite = m_renderer.GetSpriteManager().CreateSprite("test");
        m_sprite->SetScale(Cloud::Math::Float2(0.5f, 0.5f));

        m_sprite2 = m_renderer.GetSpriteManager().CreateSprite("blood");
        m_sprite2->SetScale(Cloud::Math::Float2(0.25f, 0.25f));
    }

    static float frame = 0;
    frame += 1.0f;

    std::stringstream stream;
    stream << frame;

    SetWindowText(GetActiveWindow(), stream.str().c_str());

    m_sprite->SetPosition(Cloud::Math::Float2(
        (cosf(frame * 0.0005f) + 1.0f) * 0.5f,
        (cosf(frame * 0.001f) + 1.0f) * 0.5f));

    m_sprite2->SetPosition(Cloud::Math::Float2(
        (cosf(frame * 0.001f) + 1.0f) * 0.5f,
        0.5f));
}

void RPG::TestbedApplication::Render()
{
    m_renderer.Render();
}

