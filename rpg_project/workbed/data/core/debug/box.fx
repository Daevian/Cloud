
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
    column_major float4x4 transform : TRANSFORM;
    float4 col : COLOR;
    
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

PSInput VShader(VSInput input)
{
    PSInput output;
    float4 pos = input.pos;
    pos = mul(input.transform, pos);
    pos = mul(view, pos);
    output.pos = mul(projection, pos);

    output.norm = mul((float3x3)input.transform, input.norm);
    
    output.col = input.col;
    
    return output;
}

float4 PShader(PSInput input) : SV_TARGET
{
    float light = saturate(dot(normalize(input.norm), normalize(sunDir)));
    float3 col = input.col.rgb * (ambCol + light * sunCol);
    return float4(col, input.col.a);
}


technique10 Render
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VShader()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PShader()));
    }
}