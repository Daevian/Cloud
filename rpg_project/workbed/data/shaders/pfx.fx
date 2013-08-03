static const float4 c_topLeftCorner = float4(0, 0, 0, 0);
static const float2 c_texcoords[4] = 
{ 
    float2(0,0), 
    float2(1,0),
    float2(0,1),
    float2(1,1),
};

Texture2D g_diffuseTexture : register(t0);
SamplerState g_sampler : register(s0);

cbuffer ConstantBuffer : register( b0 )
{
    matrix view;
    matrix projection;
}

struct VSInput
{
    float3 position : POSITION;
    float2 scale : SCALE;
};

struct GSInput
{
    float4 position : POSITION;
    float2 scale : SCALE;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

GSInput VShader(VSInput input)
{
    GSInput output;
    
    float4 pos = float4(input.position, 1);
    pos = mul(view, pos);
    pos = mul(projection, pos);
    
    output.position = pos;
    output.scale = input.scale;
    
    return output;
}

[maxvertexcount(4)]
void GShader(point GSInput input[1], inout TriangleStream<PSInput> triStream )
{
    PSInput output;
    
    const float4 pos = input[0].position;
    const float2 scale = input[0].scale;
    
    //CORNER 0
    output.position = pos + float4(-scale.x, scale.y, 0, 0);
    output.uv = c_texcoords[0];
    triStream.Append(output);
    
    //CORNER 1
    output.position = pos + float4(scale.x, scale.y, 0, 0);
    output.uv = c_texcoords[1];
    triStream.Append(output);
    
    //CORNER 2
    output.position = pos + float4(-scale.x, -scale.y, 0, 0);
    output.uv = c_texcoords[2];
    triStream.Append(output);
    
    //CORNER 3
    output.position = pos + float4(scale.x, -scale.y, 0, 0);
    output.uv = c_texcoords[3];
    triStream.Append(output);
    
    triStream.RestartStrip();	
}

float4 PShader(PSInput input) : SV_TARGET
{
    return g_diffuseTexture.Sample(g_sampler, input.uv);
}