
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
    float3 viewVS : TEXCOORD1;
    float4 col : COLOR0;
};

PSInput vs(VSInput input)
{
    PSInput output;
    float4 pos = input.pos;
    //pos = mul(model, pos);
    pos = mul(view, pos);

    output.viewVS = normalize(-pos.xyz);

    output.pos = mul(projection, pos);

    output.norm = input.norm;
    //output.norm = mul((float3x3)model, input.norm);

    
    
    output.col = 1;
    
    return output;
}

#define CL_PI 3.14159265359f

static const float3 ambCol = float3(0.4, 0.4, 0.4);
static const float3 sunCol = float3(1, 1, 1);
static const float3 sunDir = float3(0.0, 1.0, 0.0);
static const float sunIntensity = 100000.0f;
static const float roughness = 0.7;

static const float3 f0 = float3(1.0, 0.71, 0.29); // gold
//static const float3 f0 = float3(0.95, 0.93, 0.88); // silver

float4 ps(PSInput input) : SV_TARGET
{
    float3 vVS = normalize(input.viewVS);
    float3 n = normalize(input.norm);
    float3 nVS = mul((float3x3)view, n);
    float3 l = normalize(sunDir);
    float3 lVS = mul((float3x3)view, l);
    float3 hVS = normalize(vVS + lVS);
    float cosFactor = saturate(dot(l, n));
    float intensity = 10;// sunIntensity;
    //float3 col = input.col.rgb * (ambCol + light * sunCol);
    //float3 col = input.col.rgb * (ambCol + light * sunCol);

    //brdf
    float3 diffuse = 0;// f0 / CL_PI;

    // schlick
    //float temp = (1 - dot(lVS, nVS));
    float temp = 1 - dot(lVS, hVS); // MF
    float3 F = f0 + (1 - f0) * temp * temp * temp * temp * temp;


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
    float3 L = cosFactor * intensity * (f + diffuse);

    //L = cosFactor * (F * G * D);
    //L = f;

    return float4(L, input.col.a);// / 100000;
}