cbuffer StringConstants: register(b0)
{
	matrix viewProjection;
	float4 playerPos;
	float4 targetPos;
}

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

struct GS_OUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};


[maxvertexcount(4)]
void GS_main(point VS_OUT input[1], inout TriangleStream<GS_OUT> triStream)
{
	float width = 0.1;
	float height = 0.1;
	GS_OUT output;

		float3 newPos = float3(playerPos.x, playerPos.y, playerPos.z);
		float3 nTargetPos = targetPos.xyz;

		//nTargetPos = normalize(nTargetPos);
		//newPos = normalize(newPos);

		float3 normal = float3(targetPos  - newPos);

		//normal = normalize(normal);

		float3 rightVector = normalize(cross(normal, float3(0, 1, 0)));
			rightVector = rightVector *width;

		float3 upVector = float3(0, 1, 0);

		float4 vertc[4];
		vertc[0] = float4(targetPos.xyz - rightVector , 1);
		vertc[1] = float4(targetPos.xyz + rightVector, 1);
		vertc[2] = float4(playerPos.xyz - rightVector -upVector, 1);
		vertc[3] = float4(playerPos.xyz + rightVector -upVector, 1);

		float2 Uvs[4];
		Uvs[2] = float2(0, 0);
		Uvs[3] = float2(1, 0);
		Uvs[0] = float2(0, 1);
		Uvs[1] = float2(1, 1);

		for (uint i = 0; i < 4; i++)
		{

			output.pos = mul(vertc[i], viewProjection);
			output.worldPos = float4(0,0,0,0);
			output.tex = Uvs[i];
			output.normal = normal;
			triStream.Append(output);
		}
		triStream.RestartStrip();
}