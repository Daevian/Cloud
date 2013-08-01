static const float2 c_topLeftCorner = float2(-1, 1);
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
	matrix projection;
}

struct VSInput
{
    float2 position : POSITION;
    float2 scale : SCALE;
};

struct GSInput
{
	float2 position : POSITION;
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
	
	output.position = input.position * 2;
    output.position.y = -output.position.y;
    output.scale = input.scale * 2;
    
    return output;
}

[maxvertexcount(4)]
void GShader(point GSInput input[1], inout TriangleStream<PSInput> triStream )
{
    PSInput output;
    
    //CORNER 0
    output.position = float4(c_topLeftCorner + input[0].position, 0, 1);
    output.uv = c_texcoords[0];
    triStream.Append(output);
	
	//CORNER 1
    output.position.x += input[0].scale.x;
    output.uv = c_texcoords[1];
    triStream.Append(output);
    
    //CORNER 2
    output.position.xy -= input[0].scale;
    output.uv = c_texcoords[2];
    triStream.Append(output);
    
    //CORNER 3
    output.position.x += input[0].scale.x;
    output.uv = c_texcoords[3];
    triStream.Append(output);
	
	triStream.RestartStrip();	
}

float4 PShader(PSInput input) : SV_TARGET
{
    return g_diffuseTexture.Sample(g_sampler, input.uv);
}