#ifndef RPG_UI_ELEMENT_FACTORY_HEADER
#define RPG_UI_ELEMENT_FACTORY_HEADER

namespace RPG
{
    class UIElement;

    class UIElementFactory
    {
    public:
        UIElementFactory();

        UIElement* CreateElement(const std::string& elementName);

    private:

    };
}

#endif // RPG_UI_ELEMENT_FACTORY_HEADER