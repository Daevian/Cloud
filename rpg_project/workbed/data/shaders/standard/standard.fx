
cbuffer PerScene : register( b0 )
{
    matrix view;
    matrix projection;
	matrix viewProj;
    matrix invView;
}

cbuffer PerModel : register( b1 )
{
    matrix model;
}

cbuffer PerMaterial : register(b2)
{
    float4 f0;
    float roughness;
}

cbuffer Lighting : register(b3)
{
    struct DirectionalLight
    {
        float3  direction;
        float   intensity;
    };

    struct PointLight
    {
        float3  position;
        float   intensity;
    };

    DirectionalLight    dirLight;
    PointLight          pointLight[1];
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
    float3 viewDir : TEXCOORD1;
    float4 col : COLOR0;
};

PSInput vs(VSInput input)
{
    PSInput output;
    float4 pos = input.pos;
    pos = mul(model, pos);

    //float3 eyePos = float3(invView[3][0], invView[3][1], invView[3][2]);
    float3 eyePos = mul(invView, float4(0, 0, 0, 1)).xyz;
    output.viewDir = normalize(eyePos - pos.xyz);

    pos = mul(view, pos);

    

    output.pos = mul(projection, pos);

    output.norm = input.norm;
    output.norm = mul((float3x3)model, input.norm);
    
    output.col = 1;
    
    return output;
}

#define CL_PI 3.14159265359f

float3 brdf(float3 v, float3 l, float3 n, float3 h)
{
    float3 diffuse = f0.rgb / CL_PI;

    // schlick
    //float temp = (1 - saturate(dot(l, n)));
    float temp = 1 - saturate(dot(l, h)); // MF
    float3 F = f0.rgb + (1 - f0.rgb) * temp * temp * temp * temp * temp;
    float G = 1;

    float nm = saturate(dot(n, h));
    // phong NDF
    //float D = ((roughness + 2) / (2 * CL_PI)) *pow(saturate(dot(n, h)), roughness);

    // GGX NDF
    float a2 = roughness * roughness;
    float nm2 = nm * nm;
    float temp2 = nm2 * (a2 - 1) + 1;
    float D = a2 / (CL_PI * temp2 * temp2);

    float3 f = (F * G * D) / 4;// (4 * dot(n, l) * dot(n, v));

    return f; // metal
    //return diffuse;
    //return lerp(diffuse, f.rrr, F); // dialectric
}

float4 ps(PSInput input) : SV_TARGET
{
    float3 n = normalize(input.norm);
    float3 l = normalize(dirLight.direction);
    float3 v = normalize(input.viewDir);
    float3 h = normalize(v + l);
    float cosFactor = saturate(dot(l, n));
    float intensity = dirLight.intensity;

    float3 L = brdf(v, l, n, h) * intensity * cosFactor;

    return float4(L, input.col.a);
}