

RWTexture2D<float4> TextureOut;


[numthreads(4, 4, 1)]
void main(
    uint3 Gid  : SV_GroupID,
    uint3 DTid : SV_DispatchThreadID,
    uint3 GTid : SV_GroupThreadID,
    uint GI    : SV_GroupIndex)
{
    //TextureOut[DTid.xy] = float4(GTid.xyz / 4.0, 0);
    TextureOut[DTid.xy] = GI / 16.0;
}