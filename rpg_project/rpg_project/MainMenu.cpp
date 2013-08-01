#include "stdafx.h"
#include "MainMenu.h"
#include "GameApplication.h"
#include "UIElement.h"

void RPG::MainMenu::Initialise()
{
    auto& renderer = Application::Instance().GetRenderer();

    m_backgroundSprite = renderer.GetSpriteManager().CreateSprite("main_menu_background");
    m_newGameSprite = renderer.GetSpriteManager().CreateSprite("main_menu_new_game");
    m_continueSprite = renderer.GetSpriteManager().CreateSprite("main_menu_continue");
    m_cursorSprite = renderer.GetSpriteManager().CreateSprite("main_menu_cursor");

    UIElement* background = new UIElement();
    background->Initialise(m_backgroundSprite);
    background->SetPosition(Cloud::Math::Float2(0.0f, 0.0f));
    background->SetScale(Cloud::Math::Float2(1.0f, 1.0f));
    m_uiElements.Add(background);

    UIElement* newGame = new UIElement();
    newGame->Initialise(m_newGameSprite);
    newGame->SetPosition(Cloud::Math::Float2(0.403125f, 0.44306f));
    newGame->SetScale(Cloud::Math::Float2(0.2f, 0.089f));
    m_uiElements.Add(newGame);

    UIElement* continueE = new UIElement();
    continueE->Initialise(m_continueSprite);
    continueE->SetPosition(Cloud::Math::Float2(0.39609375f, 0.4986f));
    continueE->SetScale(Cloud::Math::Float2(0.2f, 0.089f));
    m_uiElements.Add(continueE);

    UIElement* cursor = new UIElement();
    cursor->Initialise(m_cursorSprite);
    cursor->SetPosition(Cloud::Math::Float2(0.35546875f, 0.4986f));
    cursor->SetScale(Cloud::Math::Float2(0.1f, 0.089f));
    m_uiElements.Add(cursor);
}