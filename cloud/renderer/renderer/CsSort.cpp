#include "stdafx.h"
#include "CsSort.h"
#include "RenderCore.h"
#include "GfxShader.h"


Cloud::Renderer::CsSorter::CsSorter()
    : m_shader(nullptr)
    , m_structBuffer0(nullptr)
    , m_outputBuffer(nullptr)
    , m_debugBuffer(nullptr)
{
    for (CLuint i = 0; i < c_elementCount; ++i)
    {
        m_buffer0[i].f0 = static_cast<CLfloat>(c_elementCount - i);
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
        GfxBufferDesc desc;
        ClMemZero(&desc, sizeof(desc));
        desc.elementSize = sizeof(ElementType);
        desc.elementCount = c_elementCount;
        desc.miscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

        desc.bindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.name = "cs_sort_buff0";
        desc.initialData = m_buffer0.GetBuffer();
        m_structBuffer0 = GfxCore::Instance().Create(desc);
        CL_ASSERT_NULL(m_structBuffer0);

        desc.bindFlags = D3D11_BIND_UNORDERED_ACCESS;
        desc.name = "cs_sort_buff_out";
        desc.initialData = nullptr;
        m_outputBuffer = GfxCore::Instance().Create(desc);
        CL_ASSERT_NULL(m_outputBuffer);

        // debug buffer
        desc.name = "cs_sort_buff_debug";
        desc.cpuAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.usage = D3D11_USAGE_STAGING;
        desc.bindFlags = 0;
        desc.miscFlags = 0;
        m_debugBuffer = GfxCore::Instance().Create(desc);
        CL_ASSERT_NULL(m_outputBuffer);
    }
}

void Cloud::Renderer::CsSorter::Uninit()
{
    GfxCore::Instance().Destroy(m_shader);
    GfxCore::Instance().Destroy(m_structBuffer0);
    GfxCore::Instance().Destroy(m_outputBuffer);
    GfxCore::Instance().Destroy(m_debugBuffer);
}

void Cloud::Renderer::CsSorter::Update()
{

}

void Cloud::Renderer::CsSorter::Dispatch()
{
    auto& device = GfxCore::Instance().GetRenderingDevice();
    // DISPATCH
    device.SetComputeShader(m_shader);
    device.SetBufferCS(m_structBuffer0, 0);
    device.SetUnorderedAccessView(m_outputBuffer, 0);

    device.Dispatch(c_elementCount / 64, 1, 1);

    device.SetComputeShader(nullptr);
    device.SetBufferCS(nullptr, 0);
    device.SetBufferCS(nullptr, 1);
    device.SetUnorderedAccessView((GfxBuffer*)nullptr, 0);

    // CHECK VALUES
    device.Copy(m_outputBuffer, m_structBuffer0);
    device.Copy(m_outputBuffer, m_debugBuffer);
    
    {
        GfxBufferMapDesc desc;
        ClMemZero(&desc, sizeof(desc));
        desc.buffer = m_debugBuffer;
        desc.mapType = D3D11_MAP_READ;

        GfxMappedResource mappedResource;
        
        if (device.Map(desc, mappedResource))
        {
            ElementType* elements = (ElementType*)mappedResource.data;
            CL_UNUSED(elements);

            device.Unmap(desc);
        }
    }
}
