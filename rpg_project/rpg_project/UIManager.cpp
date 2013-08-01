#include "stdafx.h"
#include "UIManager.h"

#include "UIElement.h"
#include "renderer/Renderer.h"

RPG::UIManager::UIManager()
    : m_renderer(0)
    , m_uiElements(32, 32)
    , m_backgroundSprite(0)
    , m_newGameSprite(0)
    , m_continueSprite(0)
    , m_cursorSprite(0)
{
}

CLbool RPG::UIManager::Initialise(Cloud::Renderer::Renderer& renderer)
{
    m_renderer = &renderer;

    m_backgroundSprite = m_renderer->GetSpriteManager().CreateSprite("main_menu_background");
    m_newGameSprite = m_renderer->GetSpriteManager().CreateSprite("main_menu_new_game");
    m_continueSprite = m_renderer->GetSpriteManager().CreateSprite("main_menu_continue");
    m_cursorSprite = m_renderer->GetSpriteManager().CreateSprite("main_menu_cursor");

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

    return true;
}

void RPG::UIManager::Shutdown()
{
    for (CLint i = 0, count = m_uiElements.Count(); i < count; ++i)
    {
        if (m_uiElements[i])
        {
            delete m_uiElements[i];
        }
    }
}

void RPG::UIManager::Update()
{
    CL_ASSERT(m_renderer, "UI manager not initialised!");

    
}