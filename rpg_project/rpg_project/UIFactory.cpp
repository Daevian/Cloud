#include "stdafx.h"
#include "UIFactory.h"

RPG::UIFactory::UIFactory()
{
}

RPG::UIElement* RPG::UIFactory::CreateUI(const std::string& assetName)
{
    CL_UNUSED(assetName);
    return 0;
}