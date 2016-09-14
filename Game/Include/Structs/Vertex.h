#ifndef VERTEX_H
#define VERTEX_H

#include <DirectXMath.h>

struct Vertex
{
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT2 texture;
	DirectX::XMFLOAT3 normal;

	Vertex()
	{
		position.x = 0;
		position.y = 0;
		position.z = 0;
		position.w = 0;

		texture.x = 0;
		texture.y = 0;

		normal.x = 0;
		normal.y = 0;
		normal.z = 0;
	}
	Vertex(float _x, float _y, float _z, float _U, float _V)
	{
		position.x = _x;
		position.y = _y;
		position.z = _z;
		position.w = 1;

		texture.x = _U;
		texture.y = _V;

		normal.x = 0;
		normal.y = 0;
		normal.z = 0;
	}
	Vertex(float _x, float _y, float _z, float _U, float _V, float _nX, float _nY, float _nZ)
	{
		position.x = _x;
		position.y = _y;
		position.z = _z;
		position.w = 0;

		texture.x = _U;
		texture.y = _V;

		normal.x = _nX;
		normal.y = _nY;
		normal.z = _nZ;
	}
};

#endif