

Texture2D       TDiffuse : register(t0);
SamplerState    SDiffuse : register(s0);

struct VSInput
{
    float4 position : POSITION;
    float2 uv: UV;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

PSInput VShader(VSInput input)
{
    PSInput output;

    //float4 pos = float4(input.position.xyz, 1);
    //pos = mul(view, pos);
    //pos = mul(projection, pos);
    
    output.position = input.position;
    output.uv = input.uv;

    return output;
}

float4 PShader(PSInput input) : SV_TARGET
{
    return float4(TDiffuse.Sample(SDiffuse, input.uv).rgb, 1);
}