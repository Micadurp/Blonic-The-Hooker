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

PS_OUT PS_main(VS_OUT input)
{
	PS_OUT outPut;

	float4 darkFalloff = float4(0.4f, 0.1f, 0.4f, 1);
	float4 lightFalloff = float4(0.2f, 0.1f, 0.4f, 1);

	//outPut.Diffuse = (((input.tex.y * darkFalloff) / lightFalloff) * float4(0.21f, 1, 1, 1));
	outPut.Diffuse = txDiffuse.Sample(sampAni, input.tex.xy) / float4(0.3f, 0.3f, 0.3f, 0.3f);
	outPut.Normal = float4(0.0f, 0.0f, 0.0f, 0.0f);
	outPut.WorldPos = float4(0.0f, 0.0f, 0.0f, 0.0f);
	return outPut;
};