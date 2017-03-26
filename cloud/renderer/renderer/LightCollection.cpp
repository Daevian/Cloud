#include "stdafx.h"
#include "LightCollection.h"

Cloud::Renderer::LightCollection::LightCollection()
{
}

void Cloud::Renderer::LightCollection::Initialise()
{
    m_lightCBufferData.dirLight.direction = { 0.0f, 1.0f, 0.0f };
    m_lightCBufferData.dirLight.intensity = 1.0f;

    m_lightCBufferData.pointLights[0].intensity = 1.0f;

    m_lightingCBuffer.Initialise(sizeof(m_lightCBufferData), &m_lightCBufferData);
}

void Cloud::Renderer::LightCollection::Update()
{
    m_lightingCBuffer.Reset();
    m_lightingCBuffer.UpdateConstantBuffer(&m_lightCBufferData);
}

void Cloud::Renderer::LightCollection::PopulateDebugMenu()
{
    if (ImGui::TreeNode("Directional Light"))
    {
        ImGui::InputFloat3("Direction", reinterpret_cast<CLfloat*>(m_lightCBufferData.dirLight.direction.data()));
        ImGui::SliderFloat("Intensity", &m_lightCBufferData.dirLight.intensity, 0.0f, 100000.0f, "%.3f", 10.0f);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Point Lights"))
    {
        CLuint index = 0;
        for (auto&& light : m_lightCBufferData.pointLights)
        {
            std::array<CLchar, 16> name = {0};
            sprintf_s(name.data(), name.size(), "Light %u", index++);
            if (ImGui::TreeNode(name.data()))
            {
                ImGui::InputFloat3("Position", reinterpret_cast<CLfloat*>(light.position.data()));
                ImGui::SliderFloat("Intensity", &light.intensity, 0.0f, 100000.0f, "%.3f", 10.0f);
                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
}

