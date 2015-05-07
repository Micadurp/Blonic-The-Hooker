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

cbuffer LightInfo : register(b0)
{
	float3 attenuation;
	float intensity;
	float range;
	float4 ambient;
};

cbuffer Light : register(b1)
{
	float4 lightPos[1];
	float4 lightColor[1];
};

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
	
	worldPos.w = 1.0f;
	normal.w = 0.0f;
	normal = normalize(normal);

	float3 finalColor = { 0.0f, 0.0f, 0.0f };
	float3 finalAmbient = (diffuse * ambient).xyz;

	// light to pixel vector
	float3 lightToPixel = ((lightPos[0]).xyz - (worldPos).xyz);

	// distance of the vector
	float d = length(lightToPixel);

	// pixel is too far from light source
	if (d > range) { return float4(finalAmbient, diffuse.a); }

	// turn the vector into unit length
	lightToPixel = lightToPixel / d;

	// angle of the light on the pixel surface gives amount of light
	float howMuchLight = dot((lightToPixel).xyz, (normal).xyz);

	if (howMuchLight > 0.0f)
	{
		finalColor += howMuchLight * (diffuse * lightColor[0]).xyz;

		finalColor /= attenuation[0] + (attenuation[1] * d) + (attenuation[2] * (d*d));

		finalColor *= intensity;
	}

	finalColor = saturate(finalColor + finalAmbient);

	return float4(finalColor, diffuse.a);
}