#ifndef RAY_H
#define RAY_H

#include <DirectXMath.h>

//Only use for calculations
//Use XMFLOAT instead to store data
struct Ray
{
	DirectX::XMVECTOR point0, point1;
};

#endif