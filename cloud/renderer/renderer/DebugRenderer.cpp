#include "stdafx.h"
#include "DebugRenderer.h"

#include "RenderCore.h"
#include "ShaderEffect.h"

Cloud::Renderer::DebugRenderer::DebugRenderer()
    : m_line2DEffect(0)
    , m_quadEffect(0)
    , m_boxEffect(0)
    , m_line2DsToRender(0)
    , m_quadsToRender(0)
    , m_boxesToRender(0)
{
}

CLbool Cloud::Renderer::DebugRenderer::Initialise()
{
    if (!InitialiseLine2D())
        return false;

    if (!InitialiseQuad())
        return false;

    if (!InitialiseBox())
        return false;

    RenderCore::Instance().WaitForPreviousFrame();
    return true;
}

void Cloud::Renderer::DebugRenderer::Uninitialise()
{
    m_boxInstanceBuffer.Uninitialise();
    m_boxIB.Uninitialise();
    m_boxVB.Uninitialise();   
    m_line2DVB.Uninitialise();
}

CLbool Cloud::Renderer::DebugRenderer::InitialiseLine2D()
{
    // shader
    //m_line2DEffect = RenderCore::Instance().GetEffectContainer().GetEffect("data/core/debug/line2d.eff");
    //if (!m_line2DEffect)
    //    return false;

    //// vertex buffer
    //m_line2DVB.SetVertexData(m_line2DVertices.GetBuffer());
    //m_line2DVB.SetVertexCount(m_line2DVertices.Count());
    //m_line2DVB.SetVertexSize(sizeof(Line2D::Vertex));
    //m_line2DVB.SetTopology(GfxPrimitiveTopology::Linelist);
    //if (!m_line2DVB.Initialise())
    //    return false;

    return true;
}

CLbool Cloud::Renderer::DebugRenderer::InitialiseQuad()
{
    AddQuad(ClFloat2(-0.5f, -0.5f), ClFloat2(0.5f, 0.5f), ClFloat4(1.0f, 0.0f, 0.0f, 1.0f));

    // shader
    m_quadEffect = RenderCore::Instance().GetEffectContainer().GetEffect("data/core/debug/quad.eff");
    if (!m_quadEffect)
        return false;

    // vertex buffer
    m_quadVB.SetVertexData(m_quadVertices.GetBuffer());
    m_quadVB.SetVertexCount(m_quadVertices.Count());
    m_quadVB.SetVertexSize(sizeof(Quad::Vertex));
    //m_quadVB.SetTopology(GfxPrimitiveTopology::Trianglelist);
    if (!m_quadVB.Initialise())
        return false;

    // index buffer
    Utils::StaticArray<CLuint32, Quad::c_indexCount> indices;
    for (int i = 0; i < Quad::c_maxCount; ++i)
    {
        indices[i * 6]     = i * 4 + 0;
        indices[i * 6 + 1] = i * 4 + 2;
        indices[i * 6 + 2] = i * 4 + 1;
        indices[i * 6 + 3] = i * 4 + 1;
        indices[i * 6 + 4] = i * 4 + 2;
        indices[i * 6 + 5] = i * 4 + 3;
    }

    m_quadIB.SetIndexData(indices.GetBuffer());
    m_quadIB.SetIndexCount(indices.Count());
    if (!m_quadIB.Initialise())
        return false;

    return true;
}

CLbool Cloud::Renderer::DebugRenderer::InitialiseBox()
{
    // shader
    m_boxEffect = RenderCore::Instance().GetEffectContainer().GetEffect("data/core/debug/box.eff");
    if (!m_boxEffect)
        return false;

    // vertices and indices
    Utils::StaticArray<Box::Vertex, 8> vertices;
    vertices[0].pos = ClFloat4( -1.0f, -1.0f,  1.0f, 1.0f);
    vertices[1].pos = ClFloat4(  1.0f, -1.0f,  1.0f, 1.0f);
    vertices[2].pos = ClFloat4(  1.0f,  1.0f,  1.0f, 1.0f);
    vertices[3].pos = ClFloat4( -1.0f,  1.0f,  1.0f, 1.0f);
    vertices[4].pos = ClFloat4( -1.0f, -1.0f, -1.0f, 1.0f);
    vertices[5].pos = ClFloat4(  1.0f, -1.0f, -1.0f, 1.0f);
    vertices[6].pos = ClFloat4(  1.0f,  1.0f, -1.0f, 1.0f);
    vertices[7].pos = ClFloat4( -1.0f,  1.0f, -1.0f, 1.0f);

    vertices[0].norm = ClFloat3( -1.0f, -1.0f,  1.0f);
    vertices[1].norm = ClFloat3(  1.0f, -1.0f,  1.0f);
    vertices[2].norm = ClFloat3(  1.0f,  1.0f,  1.0f);
    vertices[3].norm = ClFloat3( -1.0f,  1.0f,  1.0f);
    vertices[4].norm = ClFloat3( -1.0f, -1.0f, -1.0f);
    vertices[5].norm = ClFloat3(  1.0f, -1.0f, -1.0f);
    vertices[6].norm = ClFloat3(  1.0f,  1.0f, -1.0f);
    vertices[7].norm = ClFloat3( -1.0f,  1.0f, -1.0f);

    Utils::StaticArray<CLint32, 36> indices;
    indices[0]  = 0; indices[1]  = 1; indices[2]  = 2; indices[3]  = 2; indices[4]  = 3; indices[5]  = 0;
    indices[6]  = 3; indices[7]  = 2; indices[8]  = 6; indices[9]  = 6; indices[10] = 7; indices[11] = 3;
    indices[12] = 7; indices[13] = 6; indices[14] = 5; indices[15] = 5; indices[16] = 4; indices[17] = 7;
    indices[18] = 4; indices[19] = 0; indices[20] = 3; indices[21] = 3; indices[22] = 7; indices[23] = 4;
    indices[24] = 1; indices[25] = 0; indices[26] = 5; indices[27] = 5; indices[28] = 0; indices[29] = 4;
    indices[30] = 1; indices[31] = 5; indices[32] = 6; indices[33] = 6; indices[34] = 2; indices[35] = 1;

    // vertex buffer
    m_boxVB.SetVertexData((CLchar*)vertices.GetBuffer());
    m_boxVB.SetVertexCount(vertices.Count());
    m_boxVB.SetVertexSize(sizeof(Box::Vertex));
    //m_boxVB.SetTopology(GfxPrimitiveTopology::Trianglelist);
    if (!m_boxVB.Initialise())
        return false;

    // index buffer
    m_boxIB.SetIndexData((CLchar*)indices.GetBuffer());
    m_boxIB.SetIndexCount(indices.Count());
    if (!m_boxIB.Initialise())
        return false;

    // instance buffer
    m_boxInstanceBuffer.SetVertexData(m_boxInstances.GetBuffer());
    m_boxInstanceBuffer.SetVertexCount(m_boxInstances.Count());
    m_boxInstanceBuffer.SetVertexSize(sizeof(Box::Instance));
    if (!m_boxInstanceBuffer.Initialise())
        return false;

    return true;
}

void Cloud::Renderer::DebugRenderer::AddLine2D(const ClFloat2& start, const ClFloat2& end, const ClFloat4& colour)
{
    auto& startVertex = m_line2DVertices[m_line2DsToRender * 2];
    startVertex.position = ClFloat4(start, 0.0, 1.0);
    startVertex.colour = colour;

    auto& endVertex = m_line2DVertices[m_line2DsToRender * 2 + 1];
    endVertex.position = ClFloat4(end, 0.0, 1.0);
    endVertex.colour = colour;

    ++m_line2DsToRender;
}

void Cloud::Renderer::DebugRenderer::AddBar(const ClFloat2& position, const ClFloat2& size, const ClFloat4& colour, CLfloat value, BarDirection direction)
{
    const CLfloat c_minValue = 0.01f;
    ClFloat2 topLeft(0.0f, 0.0f);
    ClFloat2 bottomRight(0.0f, 0.0f);

    switch (direction)
    {
    case Cloud::Renderer::DebugRenderer::BarDirection::Up:
        {
            topLeft.Set(position.x - size.x, position.y - size.y);
            bottomRight.Set(position.x + size.x, position.y + size.y);

            value = Cloud::Math::Max(value, c_minValue);
            CLfloat length = topLeft.y - bottomRight.y;
            topLeft.y = bottomRight.y + length * value;
            
        } break;
    default:
        CL_ASSERT_MSG("Only 'Up' is implemented!");
    }

    AddQuad(topLeft, bottomRight, colour);
}

void Cloud::Renderer::DebugRenderer::AddQuad(const ClFloat2& topLeft, const ClFloat2& bottomRight, const ClFloat4& colour)
{
    m_quadVertices[m_quadsToRender * 4].position = ClFloat4(topLeft.x, topLeft.y, 0.0f, 1.0f);
    m_quadVertices[m_quadsToRender * 4].colour   = colour;

    m_quadVertices[m_quadsToRender * 4 + 1].position = ClFloat4(bottomRight.x, topLeft.y, 0.0f, 1.0f);
    m_quadVertices[m_quadsToRender * 4 + 1].colour   = colour;

    m_quadVertices[m_quadsToRender * 4 + 2].position = ClFloat4(topLeft.x, bottomRight.y, 0.0f, 1.0f);
    m_quadVertices[m_quadsToRender * 4 + 2].colour   = colour;

    m_quadVertices[m_quadsToRender * 4 + 3].position = ClFloat4(bottomRight.x, bottomRight.y, 0.0f, 1.0f);
    m_quadVertices[m_quadsToRender * 4 + 3].colour   = colour;

    ++m_quadsToRender;
}

void Cloud::Renderer::DebugRenderer::AddBox(const ClFloat3& position, const ClFloat3& rotation, const ClFloat3& scale, const ClFloat4& colour)
{
    ClMatrix4 matrix = ClMatrix4::Identity();
    matrix *= ClMatrix4::Scale(ClFloat4(scale, 0));
    matrix *= ClMatrix4::Rotation(ClFloat4(rotation, 0));
    
    matrix.SetCol3(ClFloat4(position, 1.0f));

    AddBox(matrix, colour);
}

void Cloud::Renderer::DebugRenderer::AddBox(const ClMatrix4& matrix, const ClFloat4& colour)
{    
    m_boxInstances[m_boxesToRender].model = matrix;
    m_boxInstances[m_boxesToRender].colour = colour;

    ++m_boxesToRender;
}

void Cloud::Renderer::DebugRenderer::RecordCommandList(ID3D12GraphicsCommandList* commandList)
{
    auto& renderCore = RenderCore::Instance();

    std::array<ID3D12DescriptorHeap*, 1> heaps = { renderCore.GetCbvHeap() };
    commandList->SetDescriptorHeaps(gsl::narrow_cast<CLuint>(heaps.size()), heaps.data());

    commandList->SetGraphicsRootSignature(renderCore.GetRootSignature());

    commandList->SetGraphicsRootConstantBufferView(0, renderCore.GetPerSceneConstBuffer().GetCurrentVersionGpuAddress());
    commandList->SetGraphicsRootConstantBufferView(1, renderCore.GetPerModelConstBuffer().GetCurrentVersionGpuAddress());

    static const auto c_cbvDescriptorSize = renderCore.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(renderCore.GetCbvHeap()->GetGPUDescriptorHandleForHeapStart(), 2, c_cbvDescriptorSize);
    commandList->SetGraphicsRootDescriptorTable(3, srvHandle);

    commandList->RSSetViewports(1, &renderCore.GetViewPort());
    commandList->RSSetScissorRects(1, &renderCore.GetScissorRect());

    auto&& rtvHandle = renderCore.GetCurrentBackBuffer();
    auto&& dsvHandle = renderCore.GetDepthStencil()->GetDsv();
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    // clear depth?
    RenderBoxes(commandList);
    RenderQuads(commandList);
    RenderLine2D();

    m_boxesToRender = 0;
    m_quadsToRender = 0;
    m_line2DsToRender = 0;
}

void Cloud::Renderer::DebugRenderer::RenderLine2D()
{
#ifdef USE_DIRECTX12
#else
    auto& renderCore = RenderCore::Instance();
    auto& device = renderCore.GetRenderingDevice();

    m_line2DVB.GPUUpdateVertexBuffer();

    device.SetEffect(m_line2DEffect);
    device.SetVertexBuffer(&m_line2DVB);

    device.Draw(m_line2DsToRender * 2);
#endif
}

void Cloud::Renderer::DebugRenderer::RenderQuads(ID3D12GraphicsCommandList* commandList)
{
#ifdef USE_DIRECTX12
    m_quadVB.GPUUpdateVertexBuffer();

    commandList->SetPipelineState(m_quadEffect->GetPipelineState());
    
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 1, &m_quadVB.GetView());
    commandList->IASetIndexBuffer(&m_quadIB.GetView());
    commandList->DrawIndexedInstanced(m_quadsToRender * 6, 1, 0, 0, 0);

#else
    auto& renderCore = RenderCore::Instance();
    auto& device = renderCore.GetRenderingDevice();

    m_quadVB.GPUUpdateVertexBuffer();
    
    device.SetEffect(m_quadEffect);
    device.SetVertexBuffer(&m_quadVB);
    device.SetIndexBuffer(&m_quadIB);

    device.DrawIndexed(m_quadsToRender * 6);
#endif
}

void Cloud::Renderer::DebugRenderer::RenderBoxes(ID3D12GraphicsCommandList* commandList)
{
#ifdef USE_DIRECTX12
    m_boxInstanceBuffer.GPUUpdateVertexBuffer();

    commandList->SetPipelineState(m_boxEffect->GetPipelineState());

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    std::array<D3D12_VERTEX_BUFFER_VIEW,2> vbs = { m_boxVB.GetView(), m_boxInstanceBuffer.GetView() };
    commandList->IASetVertexBuffers(0, gsl::narrow_cast<CLuint>(vbs.size()), vbs.data());

    commandList->IASetIndexBuffer(&m_boxIB.GetView());
    commandList->DrawIndexedInstanced(36, m_boxesToRender, 0, 0, 0);


#else
    m_boxInstanceBuffer.GpuUpdateInstanceBuffer();

    auto& renderCore = RenderCore::Instance();
    auto& device = renderCore.GetRenderingDevice();
    
    device.SetEffect(m_boxEffect);
    device.SetVertexBuffer(&m_boxVB);
    device.SetIndexBuffer(&m_boxIB);
    device.SetInstanceBuffer(&m_boxInstanceBuffer);

    device.DrawIndexedInstanced(m_boxesToRender);
    
#endif
}