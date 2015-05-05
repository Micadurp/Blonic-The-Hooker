cbuffer VertexBuffer: register(b0)
{
	matrix projectionMatrix;
};

struct VS_IN
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD;

};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;

};


VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.pos = mul(input.pos, projectionMatrix);
	output.tex = input.tex;

	return output;
}