
cbuffer PerScene : register( b0 )
{
    matrix view;
    matrix projection;
	matrix viewProj;
}

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct VS_INPUT
{
    float2 pos : POSITION;
    float2 uv  : TEXCOORD;
	float4 col : COLOR;
}; 

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
};

PS_INPUT vs(VS_INPUT input)
{
    PS_INPUT output;
    output.pos = mul(viewProj, float4(input.pos.xy, 0.f, 1.f));
    output.col = input.col;
    output.uv = input.uv;
    return output;
}

float4 ps(PS_INPUT input) : SV_Target
{
    float4 out_col = input.col * texture0.Sample(sampler0, input.uv);
    return out_col;
}