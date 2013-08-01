#include "stdafx.h"
#include "RenderRenderingDevice.h"

#include "RenderCore.h"
#include "VertexBuffer.h"
#include "ShaderEffect.h"
#include "Texture.h"

Cloud::Renderer::RenderingDevice::RenderingDevice()
: m_effect(0)
, m_vertexBuffer(0)
, m_texture(0)
{
}

CLbool Cloud::Renderer::RenderingDevice::Init()
{
    //TODO: Delete if not used!
    CL_TRACE_CHANNEL("INIT", "[RenderingDevice] Initialised!");
    return true;
}

void Cloud::Renderer::RenderingDevice::SetEffect(ShaderEffect* effect)
{
    m_effect = effect;
}

void Cloud::Renderer::RenderingDevice::SetVertexBuffer(VertexBuffer* vertexBuffer)
{
    m_vertexBuffer = vertexBuffer;
}

void Cloud::Renderer::RenderingDevice::SetTexture(Texture* texture)
{
    m_texture = texture;
}

CLbool Cloud::Renderer::RenderingDevice::Render()
{
    CL_ASSERT_NULL(m_effect);
    CL_ASSERT_NULL(m_vertexBuffer);
    CL_ASSERT_NULL(m_texture);

    m_effect->GetInputLayout().GPUSetInputLayout();
    m_vertexBuffer->GPUSetVertexBuffer();
    m_vertexBuffer->GPUSetTopology();
    m_texture->GPUSetShaderResource();
    m_texture->GPUSetSamplerState();

    m_effect->Apply();
    RenderCore::Instance().GetDeviceContext()->Draw(m_vertexBuffer->GetVertexCount(), 0);

    return true;
}
