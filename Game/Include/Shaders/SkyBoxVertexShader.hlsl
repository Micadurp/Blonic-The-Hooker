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
	input.pos.w = 0.05f;

	output.pos = mul(input.pos, mul(worldMatrix, viewProjection));

	output.tex = -float3(input.tex,1);

	return output;
}