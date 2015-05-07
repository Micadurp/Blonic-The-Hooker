#ifndef LIGHTSTRUCTURE_H
#define LIGHTSTRUCTURE_H

#include <DirectXMath.h>

using namespace DirectX;

struct LightSharedInfo
{
	XMFLOAT3 attenuation = { 0.0f, 0.3f, 0.0f };
	float intensity = 20.0f;

	XMFLOAT3 pad;
	float range = 100.0f;

	XMFLOAT4 ambient = { 0.3f, 0.3f, 0.3f, 0.3f };
};

struct LightPosColor
{
	XMFLOAT4* lightPosArray;
	XMFLOAT4* lightColorArray;
};

#endif