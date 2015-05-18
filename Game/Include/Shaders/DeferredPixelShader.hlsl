#define LIGHTS_COUNT 2

Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
Texture2D txWorldP : register(t2);

SamplerState sampAni
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct Light
{
	float4 position;
	float4 diffuse;
	float4 ambient;
	float3 attenuation;
	float range;
	float intensity;
};

cbuffer LightArray : register(b0)
{
	Light lights[LIGHTS_COUNT];
}

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};


float3 calculateLight(int index, float4 txDiff, float4 txNormal, float4 txWorldPos)
{
	float3 color = { 0.0f, 0.0f, 0.0f };
	float3 lightToPixel = lights[index].position - txWorldPos;
	float distance = length(lightToPixel);

	lightToPixel /= distance;

	float lightAmount = dot(lightToPixel, txNormal);

	if (lightAmount > 0.0f)
	{
		color += lightAmount * txDiff * lights[index].diffuse * lights[index].intensity;

		color /= lights[index].attenuation[0] + (lights[index].attenuation[1] * distance) + (lights[index].attenuation[2] * (distance * distance));
	}

	return color;
}


float4 PS_main(VS_OUT input) : SV_TARGET
{
	float4 diffuse = txDiffuse.Sample(sampAni, input.tex);
	float4 normal = txNormal.Sample(sampAni, input.tex);
	float4 worldPos = txWorldP.Sample(sampAni, input.tex);


	float3 finalColor = { 0.0f, 0.0f, 0.0f };
	float3 finalAmbient = diffuse * lights[0].ambient;

	/*
	float3 lightToPixel = lights[0].position - worldPos;
	float distance = length(lightToPixel);

	if (distance > lights[0].range) { return float4(finalAmbient, diffuse.a); }

	lightToPixel /= distance;

	float lightAmount = dot(lightToPixel, normal);

	if (lightAmount > 0.0f)
	{
		finalColor += lightAmount * diffuse * lights[0].diffuse;

		finalColor /= lights[0].attenuation[0] + (lights[0].attenuation[1] * distance) + (lights[0].attenuation[2] * (distance * distance));
	}
	*/
	

	
	for (int i = 0; i < LIGHTS_COUNT; i++)
	{
		finalColor += calculateLight(i, diffuse, normal, worldPos);
	}
	

	finalColor = saturate(finalColor + finalAmbient);

	return float4(finalColor, diffuse.a);
}