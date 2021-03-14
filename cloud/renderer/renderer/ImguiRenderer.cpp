#include "stdafx.h"
#include "ImguiRenderer.h"
#include "RenderCore.h"
#include "ShaderEffect.h"
#include <span>

Cloud::Renderer::ImguiRenderer::ImguiRenderer()
    : m_ib(sizeof(ImDrawIdx))
{
}

void Cloud::Renderer::ImguiRenderer::Initialise()
{
    m_hWnd = RenderCore::Instance().GetSettings().m_hWnd;

    if (!QueryPerformanceFrequency((LARGE_INTEGER *)&m_ticksPerSecond))
    {
        CL_ASSERT_MSG("");
    }
    if (!QueryPerformanceCounter((LARGE_INTEGER *)&m_time))
    {
        CL_ASSERT_MSG("");
    }

    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = VK_TAB;                       // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
    io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
    io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
    io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
    io.KeyMap[ImGuiKey_Home] = VK_HOME;
    io.KeyMap[ImGuiKey_End] = VK_END;
    io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
    io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
    io.KeyMap[ImGuiKey_A] = 'A';
    io.KeyMap[ImGuiKey_C] = 'C';
    io.KeyMap[ImGuiKey_V] = 'V';
    io.KeyMap[ImGuiKey_X] = 'X';
    io.KeyMap[ImGuiKey_Y] = 'Y';
    io.KeyMap[ImGuiKey_Z] = 'Z';

    io.RenderDrawListsFn = nullptr; // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
    io.ImeWindowHandle = m_hWnd;

    CreateGfxResources();
}

void Cloud::Renderer::ImguiRenderer::CreateGfxResources()
{
    m_pso = RenderCore::Instance().GetEffectContainer().FindEffect("data/core/imgui/imgui.eff");

    const CLint c_vertexCount = 5000;
    const CLint c_indexCount  = 10000;

    m_vb.SetVertexCount(c_vertexCount);
    m_vb.SetVertexSize(sizeof(ImDrawVert));
    m_vb.Initialise();

    m_ib.SetIndexCount(c_indexCount);
    m_ib.Initialise();

    CreateFontTexture();
}

void Cloud::Renderer::ImguiRenderer::CreateFontTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height, bytesPerPixel;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytesPerPixel);

    CL_ASSERT(bytesPerPixel == sizeof(CLuint32), "outputs 4 8-bit channel per pixel, otherwise imgui has changed");
    // Upload texture to graphics system
    {
        GfxTextureDesc desc = {};
        desc.name = "imgui_font";
        desc.dim = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.width = width;
        desc.height = height;
        desc.mipCount = 1;
        desc.arraySize = 1;
        desc.format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.sampleDesc.Count = 1;
        desc.heapType = D3D12_HEAP_TYPE_DEFAULT;
        desc.initialState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        desc.initialData.data = pixels;
        desc.initialData.size = width * height * bytesPerPixel;

        m_fontTexture = RenderCore::Instance().Create(desc);
    }

    // Store our identifier
    io.Fonts->TexID = static_cast<void*>(m_fontTexture.get());
}

void Cloud::Renderer::ImguiRenderer::NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    RECT rect;
    GetClientRect(m_hWnd, &rect);
    io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

    // Setup time step
    INT64 current_time;
    QueryPerformanceCounter((LARGE_INTEGER *)&current_time);
    io.DeltaTime = (float)(current_time - m_time) / m_ticksPerSecond;
    m_time = current_time;

    // Read keyboard modifiers inputs
    io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
    io.KeySuper = false;
    // io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
    // io.MousePos : filled by WM_MOUSEMOVE events
    // io.MouseDown : filled by WM_*BUTTON* events
    // io.MouseWheel : filled by WM_MOUSEWHEEL events

    // Hide OS mouse cursor if ImGui is drawing it
    if (io.MouseDrawCursor)
        SetCursor(NULL);

    // Start the frame
    ImGui::NewFrame();
}

void Cloud::Renderer::ImguiRenderer::Render(ID3D12GraphicsCommandList* commandList)
{
    ImGui::Render();

    RenderDrawLists(commandList, ImGui::GetDrawData());
}

void Cloud::Renderer::ImguiRenderer::RenderDrawLists(ID3D12GraphicsCommandList* commandList, ImDrawData* drawData)
{
    auto imguiCmdLists = std::span<ImDrawList*>(drawData->CmdLists, drawData->CmdListsCount);
    auto vertexCount = 0;
    auto indexCount = 0;
    for (auto&& imguiCmdList : imguiCmdLists)
    {
        m_vb.GPUUpdateVertexBuffer(imguiCmdList->VtxBuffer.Data, imguiCmdList->VtxBuffer.Size * sizeof(ImDrawVert), vertexCount * sizeof(ImDrawVert));
        m_ib.GPUUpdateIndexBuffer(imguiCmdList->IdxBuffer.Data, imguiCmdList->IdxBuffer.Size * sizeof(ImDrawIdx), indexCount * sizeof(ImDrawIdx));

        vertexCount += imguiCmdList->VtxBuffer.Size;
        indexCount += imguiCmdList->IdxBuffer.Size;
    }

    float L = 0.0f;
    float R = ImGui::GetIO().DisplaySize.x;
    float B = ImGui::GetIO().DisplaySize.y;
    float T = 0.0f;

    auto& renderCore = RenderCore::Instance();
    auto& perSceneConstBuffer = renderCore.GetPerSceneConstData();
    perSceneConstBuffer.view = Math::Matrix4::Identity();
    perSceneConstBuffer.invView = Math::Matrix4::Identity();
    perSceneConstBuffer.projection = Math::Matrix4::Identity();
    perSceneConstBuffer.viewProj = Math::Matrix4(
        Math::Float4(2.0f / (R - L), 0.0f, 0.0f, 0.0f),
        Math::Float4(0.0f, 2.0f / (T - B), 0.0f, 0.0f),
        Math::Float4(0.0f, 0.0f, 0.5f, 0.0f),
        Math::Float4((R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f));
    renderCore.GpuUpdatePerSceneConstBuffer();

    std::array<ID3D12DescriptorHeap*, 1> heaps = { renderCore.GetCbvHeap() };
    commandList->SetDescriptorHeaps(static_cast<CLuint>(heaps.size()), heaps.data());

    commandList->SetGraphicsRootSignature(renderCore.GetRootSignature());

    commandList->SetGraphicsRootConstantBufferView(0, renderCore.GetPerSceneConstBuffer().GetCurrentVersionGpuAddress());
    commandList->SetGraphicsRootConstantBufferView(1, renderCore.GetPerModelConstBuffer().GetCurrentVersionGpuAddress());

    static const auto c_cbvDescriptorSize = renderCore.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(renderCore.GetCbvHeap()->GetGPUDescriptorHandleForHeapStart(), 2, c_cbvDescriptorSize);
    commandList->SetGraphicsRootDescriptorTable(3, srvHandle);

    commandList->RSSetViewports(1, &renderCore.GetViewPort());

    auto&& rtvHandle = renderCore.GetCurrentBackBuffer();
    auto&& dsvHandle = renderCore.GetDepthStencil()->GetDsv();

    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    commandList->SetPipelineState(m_pso->GetPipelineState());

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    std::array<D3D12_VERTEX_BUFFER_VIEW, 1> vbs = { m_vb.GetView() };
    commandList->IASetVertexBuffers(0, static_cast<CLuint>(vbs.size()), vbs.data());
    commandList->IASetIndexBuffer(&m_ib.GetView());

    int vertexOffset = 0;
    int indexOffset = 0;
    for (auto&& imguiCmdList : imguiCmdLists)
    {
        auto imguiCmdBuffers = std::span<ImDrawCmd>(imguiCmdList->CmdBuffer.Data, imguiCmdList->CmdBuffer.Size);
        for (auto&& imguiCmdBuffer : imguiCmdBuffers)
        {
            if (imguiCmdBuffer.UserCallback)
            {
                imguiCmdBuffer.UserCallback(imguiCmdList, &imguiCmdBuffer);
            }
            else
            {
                auto scissorRect = CD3DX12_RECT(
                    (LONG)imguiCmdBuffer.ClipRect.x,
                    (LONG)imguiCmdBuffer.ClipRect.y,
                    (LONG)imguiCmdBuffer.ClipRect.z,
                    (LONG)imguiCmdBuffer.ClipRect.w);
                commandList->RSSetScissorRects(1, &renderCore.GetScissorRect());

                //ctx->PSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)&pcmd->TextureId);
                commandList->DrawIndexedInstanced(imguiCmdBuffer.ElemCount, 1, indexOffset, vertexOffset, 0);
            }

            indexOffset += imguiCmdBuffer.ElemCount;
        }

        vertexOffset += imguiCmdList->VtxBuffer.Size;
    }
}

LRESULT Cloud::Renderer::ImguiRenderer::WndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ImGuiIO& io = ImGui::GetIO();
    switch (msg)
    {
        case WM_LBUTTONDOWN:
            io.MouseDown[0] = true;
            return true;
        case WM_LBUTTONUP:
            io.MouseDown[0] = false;
            return true;
        case WM_RBUTTONDOWN:
            io.MouseDown[1] = true;
            return true;
        case WM_RBUTTONUP:
            io.MouseDown[1] = false;
            return true;
        case WM_MBUTTONDOWN:
            io.MouseDown[2] = true;
            return true;
        case WM_MBUTTONUP:
            io.MouseDown[2] = false;
            return true;
        case WM_MOUSEWHEEL:
            io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
            return true;
        case WM_MOUSEMOVE:
            io.MousePos.x = (signed short)(lParam);
            io.MousePos.y = (signed short)(lParam >> 16);
            return true;
        case WM_KEYDOWN:
            if (wParam < 256)
                io.KeysDown[wParam] = 1;
            return true;
        case WM_KEYUP:
            if (wParam < 256)
                io.KeysDown[wParam] = 0;
            return true;
        case WM_CHAR:
            // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
            if (wParam > 0 && wParam < 0x10000)
                io.AddInputCharacter((unsigned short)wParam);
            return true;
    }
    return 0;
}