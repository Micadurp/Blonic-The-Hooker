#define LIGHTS_COUNT 2

Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
Texture2D txWorldP : register(t2);

SamplerState sampAni
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct DirectionalLight
{
	float3 dir;
	float intensity;
	float4 ambient;
	float4 diffuse;
};

cbuffer LightBuffer : register(b0)
{
	DirectionalLight light;
}

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

float4 PS_main(VS_OUT input) : SV_TARGET
{
	float4 diffuse = txDiffuse.Sample(sampAni, input.tex);
	float4 normal = txNormal.Sample(sampAni, input.tex);
	float4 worldPos = txWorldP.Sample(sampAni, input.tex);

	float3 finalColor = diffuse * light.ambient;
	finalColor += saturate(dot(light.dir, normalize(normal)) * light.diffuse * diffuse * light.intensity);

	return float4(finalColor, diffuse.a);
}