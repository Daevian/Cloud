
cbuffer PerScene : register( b0 )
{
    matrix view;
    matrix projection;
	matrix viewProj;
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
    float3 viewVS : TEXCOORD1;
    float4 col : COLOR0;
};

PSInput vs(VSInput input)
{
    PSInput output;
    float4 pos = input.pos;
    pos = mul(model, pos);
    pos = mul(view, pos);

    output.viewVS = normalize(-pos.xyz);

    output.pos = mul(projection, pos);

    output.norm = input.norm;
    output.norm = mul((float3x3)model, input.norm);
    
    output.col = 1;
    
    return output;
}

#define CL_PI 3.14159265359f

float3 brdf(float3 vVS, float3 lVS, float3 nVS, float3 hVS)
{
    float3 diffuse = 0;// f0.rgb / CL_PI;

    // schlick
    //float temp = (1 - dot(lVS, nVS));
    float temp = 1 - dot(lVS, hVS); // MF
    float3 F = f0.rgb + (1 - f0.rgb) * temp * temp * temp * temp * temp;


    float G = 1;

    float nm = saturate(dot(nVS, hVS));
    // phong NDF
    //float D = ((roughness + 2) / (2 * CL_PI)) *pow(saturate(dot(nVS, hVS)), roughness);

    // GGX NDF
    float a2 = roughness * roughness;
    float nm2 = nm * nm;
    float temp2 = nm2 * (a2 - 1) + 1;
    float D = a2 / (CL_PI * temp2 * temp2);

    float3 f = (F * G * D) / 4;// (4 * dot(nVS, lVS) * dot(nVS, vVS));
    return f + diffuse;
}

float4 ps(PSInput input) : SV_TARGET
{
    float3 n = normalize(input.norm);
    float3 l = normalize(dirLight.direction);

    float3 vVS = normalize(input.viewVS);
    float3 nVS = mul((float3x3)view, n);
    float3 lVS = mul((float3x3)view, l);
    float3 hVS = normalize(vVS + lVS);
    float cosFactor = saturate(dot(l, n));
    float intensity = dirLight.intensity;

    float3 L = brdf(vVS, lVS, nVS, hVS) * intensity * cosFactor;

    return float4(L, input.col.a);
}