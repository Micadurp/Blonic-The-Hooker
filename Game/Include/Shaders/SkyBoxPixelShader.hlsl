Texture2D txDiffuse : register(t0);

SamplerState sampAni;

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float3 tex : TEXCOORD;
};

struct PS_OUT
{
	float4 Diffuse : SV_Target0;
	float4 Normal : SV_Target1;
	float4 WorldPos : SV_Target2;
};

PS_OUT PS_main(VS_OUT input) : SV_TARGET
{
	PS_OUT outPut;

	float darknessFalloff = 0.6f;
	float lightFalloff = 0.5f;

	outPut.Diffuse = ((input.tex.y + darknessFalloff) / lightFalloff) *float4(0.4f, 0.4f, 0.6f, 1);

	return outPut;
};