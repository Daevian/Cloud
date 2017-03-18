
cbuffer PerScene : register( b0 )
{
    matrix view;
    matrix projection;
}

cbuffer PerModel : register( b1 )
{
    matrix model;
}

struct VSInput
{
    float4 pos : POSITION;
    float3 norm : NORMAL;    
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 norm : TEXCOORD0;
    float4 col : COLOR0;
};


static const float3 ambCol = float3(0.4, 0.4, 0.4);
static const float3 sunCol = float3(0.5, 0.5, 0.5);
static const float3 sunDir = float3(1.0, 1.0, 1.0);

PSInput vs(VSInput input)
{
    PSInput output;
    float4 pos = input.pos;
    //pos = mul(model, pos);
    pos = mul(view, pos);
    output.pos = mul(projection, pos);

    output.norm = input.norm;
    //output.norm = mul((float3x3)model, input.norm);
    
    output.col = 1;
    
    return output;
}

float4 ps(PSInput input) : SV_TARGET
{
    float light = saturate(dot(normalize(input.norm), normalize(sunDir)));
    float3 col = input.col.rgb * (ambCol + light * sunCol);
    return float4(col, input.col.a);
}