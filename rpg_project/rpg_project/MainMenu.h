#ifndef RPG_MAIN_MENU_HEADER
#define RPG_MAIN_MENU_HEADER

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
    class MainMenu
    {
    public:
        MainMenu()
        {
        }

        void Initialise();

        void Update()
        {

        }

    private:
        Cloud::Renderer::Sprite* m_backgroundSprite;
        Cloud::Renderer::Sprite* m_newGameSprite;
        Cloud::Renderer::Sprite* m_continueSprite;
        Cloud::Renderer::Sprite* m_cursorSprite;

    };
}

#endif // RPG_MAIN_MENU_HEADER