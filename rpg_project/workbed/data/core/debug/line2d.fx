
struct VSInput
{
    float4 pos : POSITION;
    float4 col : COLOR;
    
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
};

PSInput VShader(VSInput input)
{
    PSInput output;
    output.pos = input.pos;
    output.col = input.col;
    
    return output;
}

float4 PShader(PSInput input) : SV_TARGET
{
    return input.col;
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