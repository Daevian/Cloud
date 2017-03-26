#include "stdafx.h"
#include "Scene.h"
#include "ModelRenderer.h"
#include "RenderCore.h"
#include "LightCollection.h"

Cloud::Renderer::Scene::Scene(ModelRenderer& modelRenderer, const LightCollection* lightCollection)
: m_modelRenderer(modelRenderer)
, m_lightCollection(lightCollection)
{
}

void Cloud::Renderer::Scene::AddModelInstance(const ModelInstance& modelInstance)
{
    m_instances.push_back(&modelInstance);
}

void Cloud::Renderer::Scene::RecordCommandList(ID3D12GraphicsCommandList* commandList)
{
    auto& renderCore = RenderCore::Instance();

    auto&& dsvHandle = RenderCore::Instance().GetDepthStencil()->GetDsv();
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


    std::array<ID3D12DescriptorHeap*, 1> heaps = { renderCore.GetCbvHeap() };
    commandList->SetDescriptorHeaps(gsl::narrow_cast<CLuint>(heaps.size()), heaps.data());

    commandList->SetGraphicsRootSignature(renderCore.GetRootSignature());
    commandList->SetGraphicsRootConstantBufferView(0, renderCore.GetPerSceneConstBuffer().GetCurrentVersionGpuAddress());

    if (m_lightCollection)
    {
        commandList->SetGraphicsRootConstantBufferView(4, m_lightCollection->GetCBuffer().GetCurrentVersionGpuAddress());
    }

    for (auto&& modelInstance : m_instances)
    {
        m_modelRenderer.Render(commandList, *modelInstance);
    }
}

void Cloud::Renderer::Scene::PopulateDebugMenu()
{
    CLuint index = 0;
    for (auto&& modelInstance : m_instances)
    {
        std::array<CLchar, 16> name = { 0 };
        sprintf_s(name.data(), name.size(), "Model %u", index++);
        if (ImGui::TreeNode(name.data()))
        {
            const_cast<ModelInstance*>(modelInstance)->PopulateDebugMenu();
            ImGui::TreePop();
        }
    }
}
