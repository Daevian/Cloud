#include "stdafx.h"
#include "Material.h"
#include "GfxConstantBuffer.h"
#include "ShaderEffectContainer.h"
#include "RenderCore.h"

Cloud::Renderer::Material::Material(const ResourceId& id)
    : Resource(id)
{
    m_pso = RenderCore::Instance().GetEffectContainer().FindEffect("data/shaders/standard/standard.eff");

    m_materialConstBuffer = std::make_unique<GfxConstantBuffer>();
    m_materialConstBuffer->Initialise(sizeof(MaterialData), &m_materialData);
}

void Cloud::Renderer::Material::PopulateDebugMenu()
{
    ImGui::ColorEdit4("F0", reinterpret_cast<float*>(&m_materialData.f0));
    ImGui::SliderFloat("Roughness", &m_materialData.roughness, 0.0f, 1.0f, "%.3f");

    m_materialConstBuffer->Reset();
    m_materialConstBuffer->UpdateConstantBuffer(&m_materialData);
}
