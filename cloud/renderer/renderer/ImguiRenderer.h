#pragma once

#include "GfxTexture.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ImDrawData;

namespace Cloud
{
    namespace Renderer
    {
        class ShaderEffect;
        class ImguiRenderer
        {
        public:
            ImguiRenderer();

            void Initialise();
            void NewFrame();
            void Render(ID3D12GraphicsCommandList* commandList);

            static LRESULT WndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam);

        private:
            void CreateGfxResources();
            void CreateFontTexture();
            void RenderDrawLists(ID3D12GraphicsCommandList* commandList, ImDrawData* drawData);

            INT64   m_time = 0;
            INT64   m_ticksPerSecond = 0;
            HWND    m_hWnd = 0;

            Cloud::Renderer::GfxTexture::UniquePtr m_fontTexture;
            std::shared_ptr<ShaderEffect> m_pso;
            VertexBuffer    m_vb;
            IndexBuffer     m_ib;
        };
    }
}