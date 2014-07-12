
struct GpuParticle
{
    float4 position;
    float4 velocity;
};

StructuredBuffer<GpuParticle> ParticlesIn;
RWStructuredBuffer<GpuParticle> ParticlesOut;

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    ParticlesOut[DTid.x + DTid.y].position = ParticlesIn[DTid.x + DTid.y].position;
    ParticlesOut[DTid.x + DTid.y].velocity = ParticlesIn[DTid.x + DTid.y].velocity;
}