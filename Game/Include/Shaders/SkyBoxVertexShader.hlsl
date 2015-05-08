cbuffer VertexBuffer
{
	matrix worldMatrix;
	matrix viewProjection;
};

struct VS_IN
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float3 tex : TEXCOORD;
};


VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.pos = float4(mul(input.pos, mul(worldMatrix, viewProjection)).xyz,1);

	output.tex = input.pos.xyz;


	return output;
}