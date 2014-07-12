
struct GpuParticle
{
    float4 position;
    float4 velocity;
};

struct ParticleVertex
{
    float4 pos;
    float4 scale;
};

StructuredBuffer<GpuParticle> particlesIn;
RWStructuredBuffer<ParticleVertex> vertexBuffer;

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    vertexBuffer[DTid.x + DTid.y].pos.xyz = particlesIn[DTid.x + DTid.y].position.xyz;
    vertexBuffer[DTid.x + DTid.y].scale.xy = float2(0.1, 0.1);
}