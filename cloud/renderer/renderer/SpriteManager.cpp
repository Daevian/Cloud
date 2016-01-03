#include "stdafx.h"
#include "SpriteManager.h"

#include "RenderCore.h"
#include "Sprite.h"

Cloud::Renderer::SpriteManager::SpriteManager()
{
}

void Cloud::Renderer::SpriteManager::Unload()
{
    for (auto& sprite : m_sprites)
    {
        sprite.Unload();
    }

    m_sprites.clear();
}

Cloud::Renderer::Sprite* Cloud::Renderer::SpriteManager::CreateSprite(const std::string& spriteName)
{
    m_sprites.push_back(Sprite());

    auto& newSprite = m_sprites.back();
    m_spriteFactory.CreateSprite(spriteName, newSprite);

    return &newSprite;
}

void Cloud::Renderer::SpriteManager::Render()
{
    for (auto& sprite : m_sprites)
    {
        if (sprite.GetVisible())
        {
            sprite.Render();
        }
    }
}