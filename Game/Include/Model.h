#ifndef MODEL_H
#define MODEL_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "DDSTextureLoader.h"

#include <fstream>
#include <istream>
#include <sstream>

#include <iostream>
#include <vector>


class Model
{
protected:
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

	struct Material
	{
		float Ns;
		float Ni;
		float d;
		float Tr;
		DirectX::XMFLOAT3 Tf;
		int illum;
		DirectX::XMFLOAT3 Ka;
		DirectX::XMFLOAT3 Kd;
		DirectX::XMFLOAT3 Ks;
		DirectX::XMFLOAT3 Ke;
	} material;
	
	ID3D11Buffer* meshVertexBuff;
	ID3D11Buffer* meshIndexBuff;
	int indexCount;

	ID3D11ShaderResourceView* textureShaderResource;
	ID3D11ShaderResourceView* normalShaderResource;

	int nrOfVertices;


	//change
	UINT32 vertexSize = sizeof(Vertex);

	ID3D11Buffer*   pixelShaderMaterialCB;
public:
	Model();
	~Model();


	bool Initialize(std::wstring _modelName, ID3D11Device* _device);
	void Shutdown();

	virtual void Update();
	void Render(ID3D11DeviceContext* _deviceContext);

	bool LoadObj(std::wstring _filename, ID3D11Device* _device);

	bool CreateShaders(ID3D11Device* _device);

	int GetIndexCount();
};
#endif