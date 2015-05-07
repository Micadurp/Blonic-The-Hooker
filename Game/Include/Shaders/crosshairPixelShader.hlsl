Texture2D txDiffuse : register(t0);

SamplerState sampAni
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = WRAP;
	AddressV = WRAP;

};

struct GS_OUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

struct PS_OUT
{
	float4 diffuse:SV_Target0;

};

PS_OUT PS_main(GS_OUT input) : SV_TARGET
{
	PS_OUT output;

	output.diffuse = txDiffuse.Sample(sampAni, input.tex);

	clip(output.diffuse.a - 0.25f);

	return output;
}