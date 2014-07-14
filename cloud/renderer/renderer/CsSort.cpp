#include "stdafx.h"
#include "CsSort.h"
#include "RenderCore.h"
#include "GfxShader.h"


Cloud::Renderer::CsSorter::CsSorter()
    : m_shader(nullptr)
    , m_structBuffer0(nullptr)
    , m_structBuffer1(nullptr)
    , m_outputBuffer(nullptr)
{
    for (CLuint i = 0; i < c_elementCount; ++i)
    {
        m_buffer0[i].i = 0;
        m_buffer0[i].f = 0.0f;
    }

    for (CLuint i = 0; i < c_elementCount; ++i)
    {
        m_buffer1[i].i = 1;
        m_buffer1[i].f = 1.0f;
    }
}

void Cloud::Renderer::CsSorter::Init()
{
    {
        GfxComputerShaderDesc desc;
        desc.name = "cs_sort/main";
        GfxShaderFactory::CompileShader("data/shaders/cs_sort/cs_sort.hlsl", "main", "cs_5_0", desc.shaderBlob);

        m_shader = GfxCore::Instance().Create(desc);
        CL_ASSERT_NULL(m_shader);

        desc.shaderBlob.blob->Release();
    }

    {
        GfxStructuredBufferDesc desc;
        ClMemZero(&desc, sizeof(desc));
        desc.elementSize = sizeof(ElementType);
        desc.elementCount = c_elementCount;
        
        desc.bindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.name = "cs_sort_buff0";
        desc.initialData = m_buffer0.GetBuffer();
        m_structBuffer0 = GfxCore::Instance().Create(desc);
        CL_ASSERT_NULL(m_structBuffer0);

        desc.bindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.name = "cs_sort_buff1";
        desc.initialData = m_buffer1.GetBuffer();
        m_structBuffer1 = GfxCore::Instance().Create(desc);
        CL_ASSERT_NULL(m_structBuffer1);

        desc.bindFlags = D3D11_BIND_UNORDERED_ACCESS;
        desc.name = "cs_sort_buff_out";
        desc.initialData = nullptr;
        m_outputBuffer = GfxCore::Instance().Create(desc);
        CL_ASSERT_NULL(m_outputBuffer);
    }
}

void Cloud::Renderer::CsSorter::Uninit()
{
    GfxCore::Instance().Destroy(m_shader);
    GfxCore::Instance().Destroy(m_structBuffer0);
    GfxCore::Instance().Destroy(m_structBuffer1);
    GfxCore::Instance().Destroy(m_outputBuffer);
}

void Cloud::Renderer::CsSorter::Update()
{

}

void Cloud::Renderer::CsSorter::Dispatch()
{
    // DISPATCH

    // CHECK VALUES

    // IMPLEMENT DEBUG BUFFERS
}
