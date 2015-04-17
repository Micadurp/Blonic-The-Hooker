

struct GS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

float4 PS_main(GS_OUT input) : SV_TARGET
{
	return float4(1,0,0,1);
}