// rpg_project.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GameApplication.h"
#include "renderer/Renderer.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevHInstance, LPWSTR cmdLine, CLint cmdShow)
{
    RPG::Application::Create();

    auto& application = RPG::Application::Instance();

    application.m_appInfo.m_hInstance = hInstance;
    application.m_appInfo.m_prevHInstance = prevHInstance;
    application.m_appInfo.m_cmdLine = cmdLine;
    application.m_appInfo.m_cmdShow = cmdShow;

    if (!application.Initialise())
    {
        CL_ASSERT_MSG("Game failed to initialise!");
        return 1;
    }

    application.Run();

    application.Shutdown();
    RPG::Application::Destroy();

    return 0;
}
