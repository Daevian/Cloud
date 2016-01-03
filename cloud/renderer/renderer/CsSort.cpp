#include "stdafx.h"
#include "CsSort.h"
#include "RenderCore.h"
#include "GfxShader.h"

Cloud::Renderer::CsSorter::CsSorter()
{
    for (CLuint i = 0; i < c_elementCount; ++i)
    {
        m_buffer0[i].f = static_cast<CLfloat>(c_elementCount - i);
    }
}

Cloud::Renderer::CsSorter::~CsSorter()
{
}

void Cloud::Renderer::CsSorter::Init()
{
    {
        GfxComputerShaderDesc desc;
        desc.name = "cs_sort/bitonicSort";
        GfxShaderFactory::CompileShader("data/shaders/cs_sort/cs_sort.hlsl", "bitonicSort", "cs_5_0", desc.shaderBlob);

        m_bitonicSortShader = GfxCore::Instance().Create(desc);
        CL_ASSERT_NULL(m_bitonicSortShader);

        desc.shaderBlob.blob->Release();
    }

    {
        GfxComputerShaderDesc desc;
        desc.name = "cs_sort/transpose";
        GfxShaderFactory::CompileShader("data/shaders/cs_sort/cs_sort.hlsl", "transpose", "cs_5_0", desc.shaderBlob);

        m_transposeShader = GfxCore::Instance().Create(desc);
        CL_ASSERT_NULL(m_transposeShader);

        desc.shaderBlob.blob->Release();
    }

    {
        GfxBufferDesc desc;
        ClMemZero(&desc, sizeof(desc));
        desc.bindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.elementCount = 1;

        desc.name = "cs_sort_bisort_cb";
        desc.elementSize = sizeof(BitonicSortConstBuffer);
        m_bitonicSortConstBuffer = GfxCore::Instance().Create(desc);
        CL_ASSERT_NULL(m_bitonicSortConstBuffer);
        m_bitonicSortConstBuffer = nullptr;

        desc.name = "cs_sort_transpose_cb";
        desc.elementSize = sizeof(TransposeConstBuffer);
        m_transposeConstBuffer = GfxCore::Instance().Create(desc);
        CL_ASSERT_NULL(m_transposeConstBuffer);
    }

    {
        GfxBufferDesc desc;
        ClMemZero(&desc, sizeof(desc));
        desc.elementSize = sizeof(ElementType);
        desc.elementCount = c_elementCount;
        desc.miscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

        desc.bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        desc.name = "cs_sort_buff0";
        m_structBuffer0 = GfxCore::Instance().Create(desc);
        CL_ASSERT_NULL(m_structBuffer0);

        desc.bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        desc.name = "cs_sort_buff_out";
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
    m_bitonicSortShader = nullptr;
    m_transposeShader = nullptr;
    m_bitonicSortConstBuffer = nullptr;
    m_transposeConstBuffer = nullptr;
    m_structBuffer0 = nullptr;
    m_structBuffer1 = nullptr;
    m_outputBuffer = nullptr;
    m_debugBuffer = nullptr;
}

void Cloud::Renderer::CsSorter::Update()
{
    for (CLuint i = 0; i < c_elementCount; ++i)
    {
        m_buffer0[i].f = static_cast<CLfloat>(rand() % c_elementCount);
    }

    GfxCore::Instance().GetRenderingDevice().UpdateSubresource(m_outputBuffer.get(), m_buffer0.data());
}

void Cloud::Renderer::CsSorter::Dispatch()
{
    auto& device = GfxCore::Instance().GetRenderingDevice();

    const CLuint c_matrixWidth = c_bitonicBlockSize;
    const CLuint c_matrixHeight = c_elementCount / c_matrixWidth;
    const CLuint c_transposeBlockSize = 16;

    // DISPATCH
    
    device.SetComputeShader(m_bitonicSortShader.get());
    device.SetConstantBufferCS(m_bitonicSortConstBuffer.get(), 0);
    device.SetUnorderedAccessView(m_outputBuffer.get(), 0);

    // normal bitonic sort up to block size
    for (CLuint level = 2; level <= c_bitonicBlockSize; level *= 2)
    {
        //MainTest(m_buffer0, level, level);
        UpdateBitonicSortConstBuffer(level, level);
        device.Dispatch(c_elementCount / c_bitonicBlockSize, 1, 1);
    }

    device.SetConstantBufferCS(m_transposeConstBuffer.get(), 1);

    // to reach over the block boundraries, a transpose is needed to compare values that normally aren't inside the group
    for( UINT level = (c_bitonicBlockSize * 2) ; level <= c_elementCount; level = level * 2 )
    {
        // transpose
        UpdateTransposeConstBuffer(c_matrixWidth, c_matrixHeight);
        device.SetComputeShader(m_transposeShader.get());
        device.SetUnorderedAccessView(m_structBuffer0.get(), 0);
        device.SetBufferCS(m_outputBuffer.get(), 0);
        device.Dispatch(c_matrixWidth / c_transposeBlockSize, c_matrixHeight / c_transposeBlockSize, 1);

        // sort transposed set
        // the level and level mask are reset for the transposed passes
        UpdateBitonicSortConstBuffer(level / c_bitonicBlockSize, (level & ~c_elementCount)/ c_bitonicBlockSize);
        device.SetComputeShader(m_bitonicSortShader.get());
        device.Dispatch(c_elementCount / c_bitonicBlockSize, 1, 1);
        
        device.SetUnorderedAccessView(nullptr, 0);
        device.SetBufferCS(nullptr, 0);

        // transpose back
        UpdateTransposeConstBuffer(c_matrixHeight, c_matrixWidth);
        device.SetComputeShader(m_transposeShader.get());
        device.SetUnorderedAccessView(m_outputBuffer.get(), 0);
        device.SetBufferCS(m_structBuffer0.get(), 0);
        device.SetConstantBufferCS(m_transposeConstBuffer.get(), 1);
        device.Dispatch(c_matrixHeight / c_transposeBlockSize, c_matrixWidth / c_transposeBlockSize, 1);

        // continue sorting the full group
        // the level and level mask carry on working on the entire group
        UpdateBitonicSortConstBuffer(c_bitonicBlockSize, level);
        device.SetComputeShader(m_bitonicSortShader.get());
        device.Dispatch(c_elementCount / c_bitonicBlockSize, 1, 1);

        device.SetUnorderedAccessView(nullptr, 0);
        device.SetBufferCS(nullptr, 0);
    }

    // COPY VALUES BACK
    device.Copy(m_outputBuffer.get(), m_debugBuffer.get());

    {
        GfxBufferMapDesc desc;
        ClMemZero(&desc, sizeof(desc));
        desc.buffer = m_debugBuffer.get();
        desc.mapType = D3D11_MAP_READ;

        GfxMappedResource mappedResource;
        
        if (device.Map(desc, mappedResource))
        {
            ElementType* elements = (ElementType*)mappedResource.data;
            ClMemCopy(m_buffer0.data(), elements, m_buffer0.size() * sizeof(ElementType));

            device.Unmap(desc);
        }
    }
}

void Cloud::Renderer::CsSorter::UpdateBitonicSortConstBuffer(CLuint level, CLuint levelMask)
{
    BitonicSortConstBuffer constBuffer = {level, levelMask};
    GfxCore::Instance().GetRenderingDevice().UpdateSubresource(m_bitonicSortConstBuffer.get(), &constBuffer);
}

void Cloud::Renderer::CsSorter::UpdateTransposeConstBuffer(CLuint width, CLuint height)
{
    TransposeConstBuffer constBuffer = {width, height};
    GfxCore::Instance().GetRenderingDevice().UpdateSubresource(m_transposeConstBuffer.get(), &constBuffer);
}