#ifndef HOOKSHOT_H
#define HOOKSHOT_H

#include <DirectXMath.h>

struct HookShot
{
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 point;
	float length;
	float maxLength;
	int active;
};

#endif