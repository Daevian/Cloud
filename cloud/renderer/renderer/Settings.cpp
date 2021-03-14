#include "stdafx.h"
#include "Settings.h"

Cloud::Renderer::Settings* Cloud::Renderer::Settings::s_instance = 0;

Cloud::Renderer::Settings::Settings()
{
}

Cloud::Renderer::Settings::~Settings()
{
}

bool Cloud::Renderer::Settings::Create()
{
    CL_ASSERT(s_instance == 0, "Settings already created. Can't re-create!");

    if (s_instance == 0)
    {
        s_instance = new Settings();
        return s_instance->Initialise();
    }

    return false;
}

void Cloud::Renderer::Settings::Destroy()
{
    CL_ASSERT(s_instance != 0, "Settings not created yet. Can't destroy!");

    if (s_instance != 0)
    {
        s_instance->Shutdown();
        SAFE_DELETE(s_instance);
    }
}

bool Cloud::Renderer::Settings::Initialise()
{
    std::ifstream jsonFile("data/config/settings.txt");
    
    Json::Reader reader;

    bool parseSuccessful = reader.parse(jsonFile, m_root);
    if (!parseSuccessful)
    {
        std::stringstream assertMessage;
        assertMessage << "Failed to parse user_settings file" << std::endl << reader.getFormatedErrorMessages();

        CL_TRACE_CHANNEL("ERROR", assertMessage.str().c_str());
        CL_ASSERT_MSG("Failed to parse user_settings file!");
    }

    return true;
}

void Cloud::Renderer::Settings::Shutdown()
{
    
}