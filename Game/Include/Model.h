#ifndef MODEL_H
#define MODEL_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <fstream>
#include <istream>
#include <sstream>

#include <iostream>
#include <vector>


class Model
{
protected:
	
	ID3D11Buffer* meshVertexBuff;

	ID3D11ShaderResourceView * textureShaderResource;
	std::wstring textureName;

	struct Vertex
	{
		float x, y, z;
		float U, V;

		Vertex()
		{

		}
		Vertex(float x, float y, float z, float U, float V)
		{
			this->x = x;
			this->y = y;
			this->z = z;

			this->U = U;
			this->V = V;

		}
	};

	int nrOfVertices;


	UINT32 vertexSize = sizeof(float)* 5;

	ID3D11Buffer*   geometryShaderCB;
	D3D11_BUFFER_DESC geometryShaderDesc;

	D3D11_SUBRESOURCE_DATA geometryShaderData;
public:
	Model();
	Model(std::string modelName);
	~Model();

	virtual void Update();
	void Render();
};
#endif