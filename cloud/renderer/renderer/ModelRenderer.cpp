#include "stdafx.h"
#include "ModelRenderer.h"
#include "RenderCore.h"
#include "ShaderEffect.h"
#include "Material.h"
#include "Renderer.h"

Cloud::Renderer::ModelRenderer* Cloud::Renderer::ModelRenderer::s_instance = nullptr;

struct Vertex
{
    ClFloat4 pos;
    ClFloat3 norm;
};

void GenerateSphereIbVb(std::vector<Vertex>& vertices, std::vector<CLuint32>& indices, CLint latBands, CLint longBands)
{
    const CLfloat c_radius = 0.5f;
    for (CLint latNumber = 0; latNumber <= latBands; latNumber++)
    {
        CLfloat theta = latNumber * CL_PI / static_cast<CLfloat>(latBands);
        CLfloat sinTheta = std::sin(theta);
        CLfloat cosTheta = std::cos(theta);

        for (CLint longNumber = 0; longNumber <= longBands; longNumber++)
        {
            CLfloat phi = longNumber * 2 * CL_PI / static_cast<CLfloat>(longBands);
            CLfloat sinPhi = std::sin(phi);
            CLfloat cosPhi = std::cos(phi);

            Vertex vertex = {};
            vertex.norm = ClFloat3
            {
                cosPhi * sinTheta,
                cosTheta,
                sinPhi * sinTheta,
            };

            vertex.pos = ClFloat4(vertex.norm * c_radius, 1.0f);

            //vs.Texcoord[0] = 1 - (longNumber / static_cast<CLfloat>(longBands));
            //vs.Texcoord[1] = 1 - (latNumber / static_cast<CLfloat>(latBands));

            vertices.emplace_back(vertex);
        }
    }

    for (CLint latNumber = 0; latNumber < latBands; latNumber++)
    {
        for (CLint longNumber = 0; longNumber < longBands; longNumber++)
        {
            CLint first = (latNumber * (longBands + 1)) + longNumber;
            CLint second = first + longBands + 1;

            indices.emplace_back(first);
            indices.emplace_back(first + 1);
            indices.emplace_back(second);
            
            indices.emplace_back(second);
            indices.emplace_back(first + 1);
            indices.emplace_back(second + 1);
        }
    }
}

Cloud::Renderer::Mesh::Mesh()
{
    std::vector<Vertex> vertices;
    std::vector<CLuint32> indices;

    GenerateSphereIbVb(vertices, indices, 100, 100);

    m_vb = std::make_unique<VertexBuffer>();
    m_ib = std::make_unique<IndexBuffer>();

    m_vb->SetVertexData(vertices.data());
    m_vb->SetVertexCount(static_cast<CLint>(vertices.size()));
    m_vb->SetVertexSize(sizeof(Vertex));
    m_vb->Initialise();

    m_ib->SetIndexData(indices.data());
    m_ib->SetIndexCount(static_cast<CLint>(indices.size()));
    m_ib->Initialise();
}

Cloud::Renderer::ModelInstance::ModelInstance()
{
    m_material = Renderer::GetResourceContainer().FindResource<Material>(ResourceId("metal0"));
    m_mesh = std::make_shared<Mesh>();
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
    commandList->IASetVertexBuffers(0, gsl::narrow_cast<CLuint>(vbs.size()), vbs.data());

    commandList->IASetIndexBuffer(&modelInstance.GetMesh().GetIndexBuffer().GetView());
    commandList->DrawIndexedInstanced(modelInstance.GetMesh().GetIndexBuffer().GetIndexCount(), 1, 0, 0, 0);
}

void Cloud::Renderer::ModelRenderer::PopulateDebugMenu()
{
}
