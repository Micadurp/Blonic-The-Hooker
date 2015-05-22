Texture2D txDiffuse : register(t0);

SamplerState sampAni
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = WRAP;
	AddressV = WRAP;

};

cbuffer fontColor : register(b0)
{
	float4 pixelColor;
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
	float4 diffuse : SV_Target0;

};

PS_OUT PS_main(GS_OUT input)
{
	PS_OUT output;

	float4 color;

	color = txDiffuse.Sample(sampAni, input.tex);

	if (color.r == 0.0f)
	{
		color.a = 0.0f;
	}

	else
	{
		color.a = 1.0f;
		color = color * pixelColor;
	}

	output.diffuse = color;

	return output;
}