


float4 VShader(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}

float4 PShader(float4 pos : SV_POSITION) : SV_TARGET
{
    return float4(1.0f, 1.0f, 0.0f, 1.0f);
}


technique10 Render
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VShader()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PShader()));
    }
}