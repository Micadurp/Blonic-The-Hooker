#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <DirectXMath.h>

//Only use for calculations
//Use XMFLOAT instead to store data
struct Triangle
{
	DirectX::XMVECTOR vertex0, vertex1, vertex2;
};

#endif