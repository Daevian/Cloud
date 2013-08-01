// renderer_testbed.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestbedApplication.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevHInstance, LPWSTR cmdLine, CLint cmdShow)
{
    RPG::TestbedApplication game;

    game.m_appInfo.m_hInstance = hInstance;
    game.m_appInfo.m_prevHInstance = prevHInstance;
    game.m_appInfo.m_cmdLine = cmdLine;
    game.m_appInfo.m_cmdShow = cmdShow;

    if (!game.Initialise())
    {
        CL_ASSERT_MSG("Game failed to initialise!");
        return 1;
    }

    game.Run();

    game.Shutdown();

    return 0;
}