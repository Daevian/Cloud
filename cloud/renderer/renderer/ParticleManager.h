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

    struct GpuParticle
    {
        ClFloat4 position;
        ClFloat4 velocity;
    };

    struct ParticleVertex
    {
        Math::Float4 pos;
        Math::Float4 scale;
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
        static const CLuint c_maxParticles = 256;
        Utils::StaticArray<Particle, c_maxParticles> m_particles;

        Utils::StaticArray<GpuParticle, c_maxParticles> m_gpuParticles;
        Utils::StaticArray<GpuParticle, c_maxParticles> m_gpuParticlesOut;

        VertexBuffer m_vertexBuffer;
        Utils::StaticArray<ParticleVertex, c_maxParticles> m_particleVertexData;

        ShaderEffect* m_effect;
        Texture* m_texture;


        ID3D11Buffer* m_buffer;
        ID3D11Buffer* m_bufferOut;
        ID3D11Buffer* m_bufferVertexBuffer;
        ID3D11ShaderResourceView* m_bufferSRV;
        ID3D11UnorderedAccessView* m_bufferOutUAV;
        ID3D11UnorderedAccessView* m_fillView;
        ID3D11UnorderedAccessView* m_fillOutView;
        ID3D11ShaderResourceView* m_vertexBufferView;
        ID3D11ComputeShader* m_simShader;
        ID3D11ComputeShader* m_fillShader;
    };
}
}

#endif // CLOUD_RENDERER_PARTICLE_MANAGER_HEADER