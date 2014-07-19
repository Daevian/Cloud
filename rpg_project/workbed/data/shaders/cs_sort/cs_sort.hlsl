

struct BufType
{
    float f0;
};

StructuredBuffer<BufType> Buffer0 : register(t0);
RWStructuredBuffer<BufType> BufferOut : register(u0);

[numthreads(64, 1, 1)]
void main(
    uint3 Gid  : SV_GroupID,
    uint3 DTid : SV_DispatchThreadID,
    uint3 GTid : SV_GroupThreadID,
    uint GI    : SV_GroupIndex)
{
    BufferOut[DTid.x].f0 = Buffer0[DTid.x].f0 + 1;
}