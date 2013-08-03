#include "stdafx.h"
#include "Renderer.h"

#include "RenderCore.h"

Cloud::Renderer::Renderer::Renderer()
{
    m_view = Math::Matrix4::Identity();
    m_projection = Math::Matrix4::Orthographic(1.0f, 1.0f, 0.0f, 1.0f);
}

CLbool Cloud::Renderer::Renderer::Initialise()
{
    D3D11_BUFFER_DESC constantBufferDesc;
    ClMemZero(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    constantBufferDesc.ByteWidth = sizeof(ConstantBuffer);
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = 0;
    auto result = RenderCore::Instance().GetDevice()->CreateBuffer(&constantBufferDesc, 0, &m_constantBuffer);
    if( FAILED( result) )
    {
        CL_ASSERT_MSG("Couldn't create constant buffer!");
        return false;
    }

    m_particleManager.Initialise();

    m_camera.SetCameraMatrix(ClMatrix4::Identity());
    m_camera.SetPerspective(Math::ToRadians(45.0f), 1.0f, 0.1f, 100.f);

    m_camera.SetTranslation(ClFloat4(-1.0f, 0.0f, -5.0f, 1.0f));

    return true;
}

void Cloud::Renderer::Renderer::Shutdown()
{
    m_particleManager.Uninitialise();
    m_spriteManager.Unload();

    if (m_constantBuffer)
    {
        m_constantBuffer->Release();
        m_constantBuffer = 0;
    }
}

void Cloud::Renderer::Renderer::Update()
{
    m_camera.UpdateView();

    m_particleManager.Update();
    m_particleManager.Fill();
}


void Cloud::Renderer::Renderer::Render()
{
    ConstantBuffer constantBuffer;
    constantBuffer.m_view = m_camera.GetView();
    constantBuffer.m_projection = m_camera.GetProjection();
    auto immediateContext = RenderCore::Instance().GetDeviceContext();
    immediateContext->UpdateSubresource(m_constantBuffer, 0, 0, &constantBuffer, 0, 0 );

    immediateContext->VSSetConstantBuffers( 0, 1, &m_constantBuffer);

    //m_spriteManager.Render();
    m_particleManager.Render();
    
    RenderCore::Instance().Present();
}
