#include "stdafx.h"
#include "ModelRenderer.h"
#include "RenderCore.h"
#include "ShaderEffect.h"
#include "Material.h"
#include "Renderer.h"
#include "Mesh.h"

Cloud::Renderer::ModelRenderer* Cloud::Renderer::ModelRenderer::s_instance = nullptr;

Cloud::Renderer::ModelInstance::ModelInstance(std::shared_ptr<Mesh>& mesh)
    : m_mesh(mesh)
{
    m_material = Renderer::GetResourceContainer().FindResource<Material>(ResourceId("metal0"));
}

void Cloud::Renderer::ModelInstance::PopulateDebugMenu()
{
    ImGui::Checkbox("Visible", &m_visible);
    if (ImGui::CollapsingHeader(m_material->GetId().GetDebugName()))
    {
        m_material->PopulateDebugMenu();
    }
}

Cloud::Renderer::ModelRenderer::ModelRenderer()
{
    CL_ASSERT(!s_instance, "");
    s_instance = this;
}

Cloud::Renderer::ModelRenderer::~ModelRenderer()
{
    s_instance = nullptr;
}

void Cloud::Renderer::ModelRenderer::Initialise()
{
}

void Cloud::Renderer::ModelRenderer::Render(ID3D12GraphicsCommandList* commandList, const ModelInstance& modelInstance)
{
    auto& renderCore = RenderCore::Instance();

    auto&& rtvHandle = renderCore.GetCurrentBackBuffer();
    auto&& dsvHandle = renderCore.GetDepthStencil()->GetDsv();

    if (!modelInstance.GetVisible())
    {
        return;
    }

    auto&& perModelCbufferAddress = renderCore.GetPerModelConstBuffer().UpdateConstantBuffer(&modelInstance.GetTransform());

    commandList->SetGraphicsRootConstantBufferView(1, perModelCbufferAddress);
    commandList->SetGraphicsRootConstantBufferView(2, modelInstance.GetMaterial().GetConstBuffer().GetCurrentVersionGpuAddress());

    static const auto c_cbvDescriptorSize = renderCore.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(renderCore.GetCbvHeap()->GetGPUDescriptorHandleForHeapStart(), 2, c_cbvDescriptorSize);
    commandList->SetGraphicsRootDescriptorTable(3, srvHandle);

    commandList->RSSetViewports(1, &renderCore.GetViewPort());
    commandList->RSSetScissorRects(1, &renderCore.GetScissorRect());

    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);


    commandList->SetPipelineState(modelInstance.GetMaterial().GetPso().GetPipelineState());

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    std::array<D3D12_VERTEX_BUFFER_VIEW, 1> vbs = { modelInstance.GetMesh().GetVertexBuffer().GetView() };
    commandList->IASetVertexBuffers(0, static_cast<CLuint>(vbs.size()), vbs.data());

    commandList->IASetIndexBuffer(&modelInstance.GetMesh().GetIndexBuffer().GetView());
    commandList->DrawIndexedInstanced(modelInstance.GetMesh().GetIndexBuffer().GetIndexCount(), 1, 0, 0, 0);
}

void Cloud::Renderer::ModelRenderer::PopulateDebugMenu()
{
}
