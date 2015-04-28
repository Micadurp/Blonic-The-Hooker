Texture2D txDiffuse : register(t0);

SamplerState sampAni
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = WRAP;
	AddressV = WRAP;

};

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
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

struct PS_OUT
{
	float4 diffuse:SV_Target0;
	
};

PS_OUT PS_main(GS_OUT input) : SV_TARGET
{ 
	PS_OUT outPut;
	outPut.diffuse = txDiffuse.Sample(sampAni, input.tex);

	return outPut;
};