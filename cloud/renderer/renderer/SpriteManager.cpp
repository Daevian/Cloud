#include "stdafx.h"
#include "SpriteManager.h"

#include "RenderCore.h"
#include "Sprite.h"

Cloud::Renderer::SpriteManager::SpriteManager()
: m_sprites(128, 128)
{
}

void Cloud::Renderer::SpriteManager::Unload()
{
    for (CLint i = 0; i < m_sprites.Count(); ++i)
    {
        auto sprite = m_sprites[i];
        sprite.Unload();
    }

    m_sprites.RemoveAll();
}

Cloud::Renderer::Sprite* Cloud::Renderer::SpriteManager::CreateSprite(const std::string& spriteName)
{
    m_sprites.Add(Sprite());

    auto newSprite = m_sprites.Last();
    m_spriteFactory.CreateSprite(spriteName, *newSprite);

    return newSprite;
}

void Cloud::Renderer::SpriteManager::Render()
{
    for (CLint i = 0; i < m_sprites.Count(); ++i)
    {
        auto sprite = m_sprites[i];
        if (sprite.GetVisible())
        {
            sprite.Render();
        }
    }
}