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
	struct VertexType
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT2 texture;
		DirectX::XMFLOAT3 normal;
	};
	
	ID3D11Buffer* meshVertexBuff;
	ID3D11Buffer* meshIndexBuff;
	int indexCount;

	ID3D11ShaderResourceView* textureShaderResource;
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

	//change
	UINT32 vertexSize = sizeof(VertexType);

	ID3D11Buffer*   geometryShaderCB;
	D3D11_BUFFER_DESC geometryShaderDesc;

	D3D11_SUBRESOURCE_DATA geometryShaderData;
public:
	Model();
	Model(std::wstring modelName, ID3D11Device* device);
	~Model();

	virtual void Update();
	void Render(ID3D11DeviceContext* deviceContext);

	bool LoadObj(std::wstring filename, ID3D11Device* device);
};
#endif