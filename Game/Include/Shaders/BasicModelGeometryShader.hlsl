struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

struct GS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

[maxvertexcount(3)]
void GS_main(triangle VS_OUT input[3], inout TriangleStream<GS_OUT> triStream)
{
	GS_OUT output;

	for (uint i = 0; i < 3; i++)
	{
		output.pos = input[i].pos;
		output.tex = input[i].tex;
		output.normal = input[i].normal;
		triStream.Append(output);
	}
	triStream.RestartStrip();
}