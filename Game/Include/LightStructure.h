#ifndef LIGHTSTRUCTURE_H
#define LIGHTSTRUCTURE_H

#include <DirectXMath.h>

using namespace DirectX;

struct LightInfo
{
	XMFLOAT3 attenuation = { 0.0f, 0.3f, 0.0f };
	float intensity = 20.0f;

	XMFLOAT3 pad;
	float range = 50.0f;

	XMFLOAT4 ambient = { 0.1f, 0.1f, 0.1f, 0.1f };
	XMFLOAT4 Position;
	XMFLOAT4 Color;
};

struct LightPosColor
{
};

#endif