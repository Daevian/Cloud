#include "stdafx.h"
#include "RenderRenderingDevice.h"

#include "RenderCore.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "GfxInstanceBuffer.h"
#include "GfxConstantBuffer.h"
#include "GfxTexture.h"
#include "ShaderEffect.h"
#include "Texture.h"

Cloud::Renderer::RenderingDevice::RenderingDevice()
    : m_vertexCount(0)
    , m_indexCount(0)
{
}

void Cloud::Renderer::RenderingDevice::SetEffect(ShaderEffect* effect)
{
    if (effect)
    {
        SetInputLayout(effect->GetInputLayout().GetInputLayout());
        SetVertexShader(effect->GetVertexShader());
        SetGeometryShader(effect->GetGeometryShader());
        SetPixelShader(effect->GetPixelShader());
        SetBlendState(effect->GetBlendState());
    }
    else
    {
        SetInputLayout(0);
        SetVertexShader(0);
        SetGeometryShader(0);
        SetPixelShader(0);
        SetBlendState(0);
    }
}

void Cloud::Renderer::RenderingDevice::SetInputLayout(ID3D11InputLayout* inputLayout)
{
    if (inputLayout)
    {
        RenderCore::Instance().GetContext()->IASetInputLayout(inputLayout);
    }
    else
    {
        RenderCore::Instance().GetContext()->IASetInputLayout(0);
    }
}

void Cloud::Renderer::RenderingDevice::SetVertexShader(ID3D11VertexShader* vertexShader)
{
    if (vertexShader)
    {
        RenderCore::Instance().GetContext()->VSSetShader(vertexShader, 0, 0 );
    }
    else
    {
        RenderCore::Instance().GetContext()->VSSetShader(0, 0, 0 );
    }
}

void Cloud::Renderer::RenderingDevice::SetGeometryShader(ID3D11GeometryShader* geometryShader)
{
    if (geometryShader)
    {
        RenderCore::Instance().GetContext()->GSSetShader(geometryShader, 0, 0 );
    }
    else
    {
        RenderCore::Instance().GetContext()->GSSetShader(0, 0, 0 );
    }
}

void Cloud::Renderer::RenderingDevice::SetPixelShader(ID3D11PixelShader* pixelShader)
{
    if (pixelShader)
    {
        RenderCore::Instance().GetContext()->PSSetShader(pixelShader, 0, 0 );
    }
    else
    {
        RenderCore::Instance().GetContext()->PSSetShader(0, 0, 0 );
    }
}

void Cloud::Renderer::RenderingDevice::SetComputeShader(GfxComputeShader* shader)
{
    if (shader)
    {
        CL_ASSERT_NULL(shader->GetShader());
        RenderCore::Instance().GetContext()->CSSetShader(shader->GetShader(), nullptr, 0);
    }
    else
    {
        RenderCore::Instance().GetContext()->CSSetShader(0, nullptr, 0);
    }
}

void Cloud::Renderer::RenderingDevice::SetBlendState(ID3D11BlendState* blendState)
{
    if (blendState)
    {
        RenderCore::Instance().GetContext()->OMSetBlendState(blendState, 0, 0xffffffff);
    }
    else
    {
        RenderCore::Instance().GetContext()->OMSetBlendState(0, 0, 0 );
    }
}

void Cloud::Renderer::RenderingDevice::SetVertexBuffer(VertexBuffer* vertexBuffer)
{
    if (vertexBuffer)
    {
        CLuint stride = vertexBuffer->GetVertexSize();
        CLuint offset = 0;
        ID3D11Buffer* buffer = vertexBuffer->GetBuffer();
        RenderCore::Instance().GetContext()->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
        m_vertexCount = vertexBuffer->GetVertexCount();

        SetPrimitiveTopology(vertexBuffer->GetTopology());
    }
    else
    {
        RenderCore::Instance().GetContext()->IASetVertexBuffers(0, 1, 0, 0, 0);
        SetPrimitiveTopology(GfxPrimitiveTopology::Undefined);
        m_vertexCount = 0;
    }
}

void Cloud::Renderer::RenderingDevice::SetPrimitiveTopology(GfxPrimitiveTopology topology)
{
    RenderCore::Instance().GetContext()->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(topology));
}

void Cloud::Renderer::RenderingDevice::SetIndexBuffer(IndexBuffer* indexBuffer)
{
    if (indexBuffer)
    {
        ID3D11Buffer* buffer = indexBuffer->GetBuffer();
        RenderCore::Instance().GetContext()->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
        m_indexCount = indexBuffer->GetIndexCount();
    }
    else
    {
        RenderCore::Instance().GetContext()->IASetVertexBuffers(0, 1, 0, 0, 0);
        m_indexCount = 0;
    }
}

void Cloud::Renderer::RenderingDevice::SetInstanceBuffer(GfxInstanceBuffer* instanceBuffer)
{
    if (instanceBuffer)
    {
        CLuint stride = instanceBuffer->GetInstanceSize();
        CLuint offset = 0;
        ID3D11Buffer* buffer = instanceBuffer->GetBuffer();
        RenderCore::Instance().GetContext()->IASetVertexBuffers(1, 1, &buffer, &stride, &offset);
    }
    else
    {
        RenderCore::Instance().GetContext()->IASetVertexBuffers(1, 1, 0, 0, 0);
    }
}

void Cloud::Renderer::RenderingDevice::SetConstantBuffer(GfxConstantBuffer* constantBuffer, CLuint slot)
{
    if (constantBuffer)
    {
        ID3D11Buffer* buffer = constantBuffer->getBuffer();
        CL_ASSERT_NULL(buffer);
        RenderCore::Instance().GetContext()->VSSetConstantBuffers( slot, 1, &buffer);
    }
    else
    {
        ID3D11Buffer* nullBuffer = nullptr;
        RenderCore::Instance().GetContext()->VSSetConstantBuffers( slot, 1, &nullBuffer);
    }
}

void Cloud::Renderer::RenderingDevice::SetTexturePS(GfxTexture* texture)
{
    if (texture)
    {
        auto srv = texture->GetSrv();
        CL_ASSERT_NULL(srv);
        RenderCore::Instance().GetContext()->PSSetShaderResources(0, 1, &srv);
    }
    else
    {
        ID3D11ShaderResourceView* nullView = nullptr;
        RenderCore::Instance().GetContext()->PSSetShaderResources(0, 1, &nullView);
    }
}

void Cloud::Renderer::RenderingDevice::SetBufferCS(GfxBuffer* buffer, CLuint slot)
{
    if (buffer)
    {
        auto srv = buffer->GetSrv();
        CL_ASSERT_NULL(srv);
        RenderCore::Instance().GetContext()->CSSetShaderResources(slot, 1, &srv);
    }
    else
    {
        ID3D11ShaderResourceView* nullView = nullptr;
        RenderCore::Instance().GetContext()->CSSetShaderResources(slot, 1, &nullView);
    }
}

void Cloud::Renderer::RenderingDevice::SetUnorderedAccessView(GfxBuffer* buffer, CLuint slot)
{
    if (buffer)
    {
        auto uav = buffer->GetUav();
        CL_ASSERT_NULL(uav);
        RenderCore::Instance().GetContext()->CSSetUnorderedAccessViews(slot, 1, &uav, nullptr);
    }
    else
    {
        ID3D11UnorderedAccessView* nullView = nullptr;
        RenderCore::Instance().GetContext()->CSSetUnorderedAccessViews(slot, 1, &nullView, nullptr);
    }
}

void Cloud::Renderer::RenderingDevice::SetSamplerState(ID3D11SamplerState* samplerState, CLuint slot)
{
    RenderCore::Instance().GetContext()->PSSetSamplers(slot, 1, &samplerState);
}

void Cloud::Renderer::RenderingDevice::Copy(GfxBuffer* source, GfxBuffer* dest)
{
    CL_ASSERT_NULL(source);
    CL_ASSERT_NULL(dest);

    RenderCore::Instance().GetContext()->CopyResource(dest->GetBuffer(), source->GetBuffer());
}

CLbool Cloud::Renderer::RenderingDevice::Map(const GfxBufferMapDesc& desc, GfxMappedResource& mappedResource)
{
    CL_ASSERT_NULL(desc.buffer);

    D3D11_MAPPED_SUBRESOURCE mappedRes;
    auto hr = RenderCore::Instance().GetContext()->Map(desc.buffer->GetBuffer(), desc.subresource, desc.mapType, desc.mapFlags, &mappedRes);
    if (FAILED(hr))
    {
        CL_ASSERT_MSG("Failed to map the buffer!");
        return false;
    }

    mappedResource.data         = mappedRes.pData;
    mappedResource.rowPitch     = mappedRes.RowPitch;
    mappedResource.depthPitch   = mappedRes.DepthPitch;
    return true;
}

void Cloud::Renderer::RenderingDevice::Unmap(const GfxBufferMapDesc& desc)
{
    CL_ASSERT_NULL(desc.buffer);

    RenderCore::Instance().GetContext()->Unmap(desc.buffer->GetBuffer(), desc.subresource);
}

void Cloud::Renderer::RenderingDevice::Draw(CLint vertexCount)
{
    vertexCount = vertexCount < 0 ? m_vertexCount : vertexCount;
    RenderCore::Instance().GetContext()->Draw(vertexCount, 0);
}

void Cloud::Renderer::RenderingDevice::DrawInstanced(CLuint instanceCount, CLint vertexCount)
{
    vertexCount = vertexCount < 0 ? m_vertexCount : vertexCount;
    RenderCore::Instance().GetContext()->DrawInstanced(vertexCount, instanceCount, 0, 0);
}

void Cloud::Renderer::RenderingDevice::DrawIndexed(CLint indexCount)
{
    indexCount = indexCount < 0 ? m_indexCount : indexCount;
    RenderCore::Instance().GetContext()->DrawIndexed(indexCount, 0, 0);
}

void Cloud::Renderer::RenderingDevice::DrawIndexedInstanced(CLuint instanceCount, CLint indexCount)
{
    indexCount = indexCount < 0 ? m_indexCount : indexCount;
    RenderCore::Instance().GetContext()->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0);
}

void Cloud::Renderer::RenderingDevice::Dispatch(const CLuint threadGroupCountX, const CLuint threadGroupCountY, const CLuint threadGroupCountZ)
{
    RenderCore::Instance().GetContext()->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}
