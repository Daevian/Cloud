#ifndef RPG_UI_FACTORY_HEADER
#define RPG_UI_FACTORY_HEADER

#include "UIElementFactory.h"

namespace RPG
{
    class UIElement;

    class UIFactory
    {
    public:
        UIFactory();

        UIElement* CreateUI(const std::string& assetName);

    private:
        UIElementFactory m_uiElementFactory;

    };
}

#endif // RPG_UI_FACTORY_HEADER