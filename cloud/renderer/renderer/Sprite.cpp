#include "stdafx.h"
#include "Sprite.h"

#include "RenderCore.h"
#include "ShaderEffect.h"
#include "Texture.h"
#include "Settings.h"

Cloud::Renderer::Sprite::Sprite()
:m_effect(0)
,m_texture(0)
,m_visible(true)
{
    m_vertex.pos.Set(0.0f, 0.0f);
    m_vertex.scale.Set(1.0f, 1.0f);
}

Cloud::Renderer::Sprite::Sprite(const Sprite& sprite)
:m_vertexBuffer(sprite.m_vertexBuffer)
,m_effect(sprite.m_effect)
,m_texture(sprite.m_texture)
,m_vertex(sprite.m_vertex)
,m_visible(sprite.m_visible)
{
}

CLbool Cloud::Renderer::Sprite::Init(Texture* texture)
{
    m_effect = RenderCore::Instance().GetEffectContainer().GetEffect("data/effects/sprite.effect");
    m_texture = texture;

    m_vertexBuffer.SetVertexCount(1);
    m_vertexBuffer.SetVertexSize(sizeof(SpriteVertex));
    m_vertexBuffer.SetVertexData((CLchar*)&m_vertex);
    m_vertexBuffer.SetTopology(GfxPrimitiveTopology::Pointlist);

    if (!m_vertexBuffer.Initialise()) return false;

    return true;
}

void Cloud::Renderer::Sprite::Unload()
{
    m_vertexBuffer.Uninitialise();
}

void Cloud::Renderer::Sprite::Render()
{
    RenderingDevice& renderingDevice = RenderCore::Instance().GetRenderingDevice();

    m_vertexBuffer.GPUUpdateVertexBuffer();

    renderingDevice.SetTexture(m_texture ? m_texture->GetTexture() : nullptr);
    renderingDevice.SetSamplerState(m_texture ? m_texture->GetSamplerState() : nullptr, 0);
    renderingDevice.SetEffect(m_effect);
    renderingDevice.SetVertexBuffer(&m_vertexBuffer);

    renderingDevice.Draw();
}
