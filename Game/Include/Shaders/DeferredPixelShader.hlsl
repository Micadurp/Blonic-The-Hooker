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

cbuffer LightShared : register(b0)
{
	float4 ambient;
	float3 attenuation;
	float range;
	float intensity;
}

struct Light
{
	float4 position;
	float4 diffuse;
};

cbuffer LightArray : register(b1)
{
	Light lights[LIGHTS_COUNT];
}


/*
struct DirectionalLight
{
	float3 dir;
	float intensity;
	float4 ambient;
	float4 diffuse;
};


cbuffer lightColor : register(b0)
{
	float4 lightDiff[LIGHTS_COUNT];
}

cbuffer lightPos : register(b1)
{
	float4 lightPos[LIGHTS_COUNT];
}

cbuffer LightBuffer : register(b3)
{
	DirectionalLight light;
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
	float3 finalAmbient = diffuse * ambient;

	for (int i = 0; i < LIGHTS_COUNT; i++)
	{
		float3 lightToPixel = lights[i].position - worldPos;

		float d = length(lightToPixel);

		lightToPixel /= d;

		float lightAmount = dot(lightToPixel, normal);

		if (lightAmount > 0.0f)
		{
			finalColor += lightAmount * (diffuse * lights[i].diffuse);
			
			finalColor /= attenuation[0] + (attenuation[1] * d) + (attenuation[2] * (d*d));
		}
	}

	finalColor = saturate(finalColor + finalAmbient);

	return float4(finalColor, diffuse.a);


	/*
	float lightIntensity1, lightIntensity2;
	float4 finalColor, color1, color2;

	finalColor = diffuse * light.ambient;
	

	// Calculate the different amounts of light on this pixel based on the positions of the lights.
	lightIntensity1 = saturate(dot(normal, lightPos[0]));
	lightIntensity2 = saturate(dot(normal, lightPos[1]));

	// Determine the diffuse color amount of each of the four lights.
	color1 = lightDiff[0] * lightIntensity1;
	color2 = lightDiff[1] * lightIntensity2;


	// Multiply the texture pixel by the combination of all four light colors to get the final result.
	finalColor += saturate(color1 + color2) * diffuse;

	finalColor += saturate(light.diffuse * diffuse);

	return finalColor;
	*/
}