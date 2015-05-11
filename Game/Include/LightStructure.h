#ifndef LIGHTSTRUCTURE_H
#define LIGHTSTRUCTURE_H

#include <DirectXMath.h>

using namespace DirectX;

struct LightInfo
{
	XMFLOAT3 attenuation = { 0.0f, 0.0f, 0.2f };
	float intensity = 100.0f;

	XMFLOAT3 pad;
	float range = 1000.0f;

	XMFLOAT4 ambient = { 0.1f, 0.1f, 0.1f, 0.1f };
	XMFLOAT4 Position;
	XMFLOAT4 Color;
};

struct SceneLightsStruct
{
	LightInfo* lights;
};

#endif