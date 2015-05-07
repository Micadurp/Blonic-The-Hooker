Texture2D txDiffuse : register(t0);

SamplerState sampAni;

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float3 tex : TEXCOORD;
};

struct PS_OUT
{
	float4 Diffuse:SV_Target0;
};

PS_OUT PS_main(VS_OUT input) : SV_TARGET
{
	PS_OUT outPut;
	outPut.Diffuse = txDiffuse.Sample(sampAni, input.tex);

	return outPut;
};