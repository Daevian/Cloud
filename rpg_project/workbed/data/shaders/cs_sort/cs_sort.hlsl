
#define BITONIC_BLOCK_SIZE 512

struct ElementType
{
    float f;
};

cbuffer BitonicSortConstBuffer : register( b0 )
{
    unsigned int g_level;
    unsigned int g_levelMask;
};

cbuffer TransposeConstBuffer : register( b1 )
{
    unsigned int g_width;
    unsigned int g_height;
};

groupshared float s_sharedData[BITONIC_BLOCK_SIZE];

StructuredBuffer<ElementType> InputBuffer : register(t0);
RWStructuredBuffer<ElementType> DataBuffer : register(u0);

[numthreads(BITONIC_BLOCK_SIZE, 1, 1)]  // thread group sizes
void bitonicSort(
    uint3 Gid  : SV_GroupID,            // id of thread group (from Dispach(x, y, z)
    uint3 DTid : SV_DispatchThreadID,   // id of unique thread
    uint3 GTid : SV_GroupThreadID,      // id of thread in a group (from numthreads(x, y, z)
    uint  GI   : SV_GroupIndex)         // the flattened id of thread in the group
{
    // Load shared data
    s_sharedData[GI] = DataBuffer[DTid.x].f;
    GroupMemoryBarrierWithGroupSync();

    // Sort the shared data
    for (unsigned int j = g_level >> 1 ; j > 0 ; j >>= 1)
    {
        float first = s_sharedData[GI & ~j];
        float second = s_sharedData[GI | j];
        bool mask = (bool)(g_levelMask & DTid.x);
        float res0 = s_sharedData[GI ^ j];
        float res1 = s_sharedData[GI];
        float result = (( first <= second) == mask) ? res0 : res1;
        GroupMemoryBarrierWithGroupSync();
        s_sharedData[GI] = result;
        GroupMemoryBarrierWithGroupSync();
    }

    // Store shared data
    DataBuffer[DTid.x].f = s_sharedData[GI];
}

#define TRANSPOSE_BLOCK_SIZE 16

[numthreads(TRANSPOSE_BLOCK_SIZE, TRANSPOSE_BLOCK_SIZE, 1)]  // thread group sizes
void transpose(
    uint3 Gid  : SV_GroupID,            // id of thread group (from Dispach(x, y, z)
    uint3 DTid : SV_DispatchThreadID,   // id of unique thread
    uint3 GTid : SV_GroupThreadID,      // id of thread in a group (from numthreads(x, y, z)
    uint  GI   : SV_GroupIndex)         // the flattened id of thread in the group
{
    DataBuffer[DTid.x * g_height + DTid.y].f = InputBuffer[DTid.y * g_width + DTid.x].f;
}