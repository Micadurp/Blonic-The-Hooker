Texture2D txDiffuse : register(t0);

SamplerState sampAni
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = WRAP;
	AddressV = WRAP;

};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 PS_main(VS_OUT input) : SV_TARGET
{
	float4 diffuse = float4(0, 1, 1,1);//txDiffuse.Sample(sampAni, input.tex);
	return diffuse;
}