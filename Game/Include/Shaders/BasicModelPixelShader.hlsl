Texture2D txDiffuse : register(t0);

SamplerState sampAni;

cbuffer MaterialConstant: register(b0)
{
	float Ns;
	float Ni;
	float d;
	float Tr;
	float3 Tf;
	int illum;
	float3 Ka;
	float3 Kd;
	float3 Ks;
	float3 Ke;
}

struct GS_OUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

struct PS_OUT
{
	float4 Diffuse : SV_Target0;
	float4 Normal : SV_Target1;
	float4 WorldPos : SV_Target2;
};

PS_OUT PS_main(GS_OUT input)
{ 
	PS_OUT output;
	output.Diffuse = float4(txDiffuse.Sample(sampAni, input.tex).xyz,0);
	output.Normal = normalize(float4(input.normal, 0.0f));
	output.WorldPos = input.worldPos;

	return output;
};