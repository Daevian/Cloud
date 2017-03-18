#include "stdafx.h"
#include "ShaderEffectContainer.h"

#include "ShaderEffect.h"

Cloud::Renderer::ShaderEffectContainer::ShaderEffectContainer()
{
}

Cloud::Renderer::ShaderEffectContainer::~ShaderEffectContainer()
{
}

std::shared_ptr<Cloud::Renderer::ShaderEffect> Cloud::Renderer::ShaderEffectContainer::FindEffect(const std::string& effectPath)
{
    std::lock_guard<std::mutex> lock(m_cacheLock);

    auto cachedEffect = m_effects[effectPath].lock();

    if (!cachedEffect)
    {
        cachedEffect = std::make_shared<ShaderEffect>();
        m_effects[effectPath] = cachedEffect;
        cachedEffect->Load(effectPath);
    }

    return cachedEffect;
}

Cloud::Renderer::ShaderEffect* Cloud::Renderer::ShaderEffectContainer::GetEffect(const std::string& effectPath)
{
    return LoadEffect(effectPath);
}

Cloud::Renderer::ShaderEffect* Cloud::Renderer::ShaderEffectContainer::LoadEffect(const std::string& effectPath)
{
    ShaderEffect* effect = new ShaderEffect();
    effect->Load(effectPath);

    return effect;
}