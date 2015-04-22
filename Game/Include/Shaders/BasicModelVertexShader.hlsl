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
	float4 pos : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};


VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.pos = mul(input.pos, worldMatrix);
	output.pos = mul(output.pos, viewProjection);

	output.tex = input.tex;
	output.normal = input.normal;

	return output;
}