#ifndef RPG_UI_MANAGER_HEADER
#define RPG_UI_MANAGER_HEADER

#include "UIFactory.h"

namespace Cloud
{
    namespace Renderer
    {
        class Renderer;
        class Sprite;
    }
}

namespace RPG
{
    class UIElement;

    class UIManager
    {
    public:
        UIManager();

        CLbool Initialise(Cloud::Renderer::Renderer& renderer);
        void Shutdown();

        void Update();

    private:
        UIFactory m_uiFactory;

        Cloud::Utils::DynamicArray<UIElement*> m_uiElements;
        Cloud::Renderer::Renderer* m_renderer;

        Cloud::Renderer::Sprite* m_backgroundSprite;
        Cloud::Renderer::Sprite* m_newGameSprite;
        Cloud::Renderer::Sprite* m_continueSprite;
        Cloud::Renderer::Sprite* m_cursorSprite;
    };
}

#endif // RPG_UI_MANAGER_HEADER