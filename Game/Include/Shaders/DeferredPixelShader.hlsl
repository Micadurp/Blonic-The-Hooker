#define LIGHTS_COUNT 1

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

cbuffer LightInfo: register(b0)
{
	float3 attenuation;
	float intensity;
	float3 pad;
	float range;
	float4 ambient;
	float4 lightPos;
	float4 lightColor;
};

/*
cbuffer LightBuffer : register(b0)
{
	LightInfo lights;
}
*/

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

float4 PS_main(VS_OUT input) : SV_TARGET
{
	float4 diffuse = txDiffuse.Sample(sampAni, input.tex);
	float4 normal = txNormal.Sample(sampAni, input.tex);
	float4 worldPos = txWorldP.Sample(sampAni, input.tex);

	float3 finalColor = { 0.0f, 0.0f, 0.0f };
	float3 finalAmbient = (diffuse * ambient).xyz;

	//for (int i = 0; i < LIGHTS_COUNT; i++)
	//{
		// light to pixel vector
		float3 lightToPixel = (lightPos).xyz - (worldPos).xyz;

		// distance of the vector
		float d = length(lightToPixel);

		// pixel is too far from light source
		if (d > range)
		{
			return float4(finalAmbient, diffuse.a);
		}

		// turn the vector into unit length
		lightToPixel = lightToPixel / d;

		// angle of the light on the pixel surface gives amount of light
		float howMuchLight = dot(lightToPixel, normal);

		if (howMuchLight > 0.0f)
		{
			finalColor += howMuchLight * diffuse * lightColor * intensity;

			finalColor /= attenuation[0] + (attenuation[1] * d) + (attenuation[2] * (d*d));
		}

		finalColor = saturate(finalColor + finalAmbient);
	//}

	return float4(finalColor, diffuse.a);
}