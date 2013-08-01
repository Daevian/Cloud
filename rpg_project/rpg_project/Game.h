#ifndef RPG_GAME_HEADER
#define RPG_GAME_HEADER

#include "MainMenu.h"

namespace RPG
{
    class Game
    {
    public:
        Game()
        {

        }

        void Update()
        {
            m_mainMenu.Update();
        }

    private:
        MainMenu m_mainMenu;

    };
}

#endif // RPG_GAME_HEADER