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


        Dx::UniquePtr<ID3D11Buffer> m_buffer;
        Dx::UniquePtr<ID3D11Buffer> m_bufferOut;
        Dx::UniquePtr<ID3D11Buffer> m_bufferVertexBuffer;
        Dx::UniquePtr<ID3D11ShaderResourceView> m_bufferSRV;
        Dx::UniquePtr<ID3D11UnorderedAccessView> m_bufferOutUAV;
        Dx::UniquePtr<ID3D11UnorderedAccessView> m_fillView;
        Dx::UniquePtr<ID3D11UnorderedAccessView> m_fillOutView;
        Dx::UniquePtr<ID3D11ShaderResourceView> m_vertexBufferView;
        Dx::UniquePtr<ID3D11ComputeShader> m_simShader;
        Dx::UniquePtr<ID3D11ComputeShader> m_fillShader;
    };
}
}

#endif // CLOUD_RENDERER_PARTICLE_MANAGER_HEADER