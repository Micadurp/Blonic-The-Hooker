#ifndef LIGHTSTRUCTURE_H
#define LIGHTSTRUCTURE_H

#include <DirectXMath.h>

using namespace DirectX;

struct DirectionalLight
{
	XMFLOAT3 direction;
	float pad;

	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};

struct PointLight
{
	XMFLOAT4 position;
	XMFLOAT4 diffuse;
	XMFLOAT4 ambient = { 0.2f, 0.2f, 0.2f, 0.2f };

	XMFLOAT3 attenuation = { 0.0f, 0.0f, 0.3f };
	float range = 1000.0f;

	float intensity = 200.0f;
	XMFLOAT3 pad;
};

#endif