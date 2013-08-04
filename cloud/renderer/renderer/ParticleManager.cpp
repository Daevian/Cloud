#include "stdafx.h"
#include "ParticleManager.h"

#include "RenderCore.h"
#include "ShaderEffect.h"
#include "Texture.h"

Cloud::Renderer::ParticleManager::ParticleManager()
{
}

Cloud::Renderer::ParticleManager::~ParticleManager()
{
}

CLbool Cloud::Renderer::ParticleManager::Initialise()
{
    m_effect = RenderCore::Instance().GetEffectContainer().GetEffect("data/effects/pfx.effect");
    m_texture = RenderCore::Instance().GetTextureContainer().GetTexture("data/textures/main_menu_cursor.dds");

    m_vertexBuffer.SetVertexCount(m_particles.Count());
    m_vertexBuffer.SetVertexSize(sizeof(ParticleVertex));
    m_vertexBuffer.SetVertexData((CLchar*)&m_particleVertexData);
    m_vertexBuffer.SetTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

    if (!m_vertexBuffer.Initialise()) return false;

    for (CLuint i = 0; i < m_particles.Count(); i++)
    {
        CLfloat velX = (ClRandFloat() - 0.5f) * 0.05f;
        CLfloat velY = (ClRandFloat() - 0.5f) * 0.05f;
        CLfloat velZ = (ClRandFloat() - 0.5f) * 0.05f;

        m_particles[i].position = ClFloat4(0.0f, 0.0f, 0.0f, 1.0f);
        m_particles[i].velocity = ClFloat4(velX, velY, velZ, 0.0f);
    }

    return true;
}

void Cloud::Renderer::ParticleManager::Uninitialise()
{
    m_vertexBuffer.Uninitialise();
}

void Cloud::Renderer::ParticleManager::Update(CLfloat timeStep)
{
    ClFloat4 timeStepVector(timeStep);

    for (CLuint particleIndex = 0, count = m_particles.Count(); particleIndex < count; ++particleIndex)
    {
        Particle& particle = m_particles[particleIndex];
        particle.position += particle.velocity * timeStepVector;
    }
}

void Cloud::Renderer::ParticleManager::Fill()
{
    for (CLuint i = 0; i < m_particles.Count(); i++)
    {
        Particle& particle = m_particles[i];
        ParticleVertex& vertex = m_particleVertexData[i];

        vertex.pos.x = particle.position.x;
        vertex.pos.y = particle.position.y;
        vertex.pos.z = particle.position.z;
        vertex.scale.x = 0.1f;
        vertex.scale.y = 0.1f;
    }

    m_vertexBuffer.GPUUpdateVertexBuffer();
}

void Cloud::Renderer::ParticleManager::Render()
{
    RenderingDevice& renderingDevice = RenderCore::Instance().GetRenderingDevice();

    //m_vertexBuffer.GPUUpdateVertexBuffer();

    renderingDevice.SetTexture(m_texture);
    renderingDevice.SetEffect(m_effect);
    renderingDevice.SetVertexBuffer(&m_vertexBuffer);

    renderingDevice.Render();
}