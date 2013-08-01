#include "stdafx.h"
#include "ShaderEffectContainer.h"

#include "ShaderEffect.h"

Cloud::Renderer::ShaderEffectContainer::ShaderEffectContainer()
:m_effects(128, 128)
{
}

Cloud::Renderer::ShaderEffectContainer::~ShaderEffectContainer()
{
    for (CLint i = 0; i < m_effects.Count(); ++i)
    {
        m_effects[i]->Unload();
        delete m_effects[i];
    }

    m_effects.RemoveAll();
}

Cloud::Renderer::ShaderEffect* Cloud::Renderer::ShaderEffectContainer::GetEffect(const std::string& effectPath)
{
    for (CLint i = 0; i < m_effects.Count(); ++i)
    {
        if (m_effects[i]->GetPath() == effectPath)
        {
            return m_effects[i];
        }
    }

    return LoadEffect(effectPath);
}

Cloud::Renderer::ShaderEffect* Cloud::Renderer::ShaderEffectContainer::LoadEffect(const std::string& effectPath)
{
    ShaderEffect* effect = new ShaderEffect();
    effect->Load(effectPath);
    m_effects.Add(effect);

    return effect;
}