#include "stdafx.h"
#include "UIElement.h"

#include "renderer/Sprite.h"

RPG::UIElement::UIElement()
{
}

CLbool RPG::UIElement::Initialise(Cloud::Renderer::Sprite* sprite)
{
    m_sprite = sprite;
    return true;
}

void RPG::UIElement::SetPosition(const Cloud::Math::Float2& position)
{
    if (m_sprite)
    {
        m_sprite->SetPosition(position);
    }
}

void RPG::UIElement::SetScale(const Cloud::Math::Float2& scale)
{
    if (m_sprite)
    {
        m_sprite->SetScale(scale);
    }
}