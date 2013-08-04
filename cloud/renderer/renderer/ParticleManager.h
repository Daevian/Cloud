#ifndef CLOUD_RENDERER_PARTICLE_MANAGER_HEADER
#define CLOUD_RENDERER_PARTICLE_MANAGER_HEADER

#include "VertexBuffer.h"

namespace Cloud
{
namespace Renderer
{
    class ShaderEffect;
    class Texture;

    struct Particle
    {
        ClFloat4 position;
        ClFloat4 velocity;
    };

    struct ParticleVertex
    {
        Math::Float3 pos;
        Math::Float2 scale;
    };

    class ParticleManager
    {
    public:
        ParticleManager();
        ~ParticleManager();

        CLbool Initialise();
        void Uninitialise();

        void Update(CLfloat timeStep);
        void Fill();
        void Render();

    private:
        static const CLuint s_maxParticles = 102400;
        Utils::StaticArray<Particle, s_maxParticles> m_particles;

        VertexBuffer m_vertexBuffer;
        Utils::StaticArray<ParticleVertex, s_maxParticles> m_particleVertexData;

        ShaderEffect* m_effect;
        Texture* m_texture;
    };
}
}

#endif // CLOUD_RENDERER_PARTICLE_MANAGER_HEADER