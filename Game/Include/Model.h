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

using namespace DirectX;

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
		float Ns; //Specular Power
		float Ni; //Optical Density, 0-10, jobbar med transparanta objekt
		float d; // Transparacy
		float Tr; // Transparacy igen används utav ett annat filformat
		DirectX::XMFLOAT3 Tf; //Transmission Filter, filtrerar ljus som passerar igenom transparanta objekt
		int illum; // Ljussättnings tjofräs , 0-10
		DirectX::XMFLOAT3 Ka; //Ambient Color, detta är färgen på ett objekt när inget ljus är på den
		DirectX::XMFLOAT3 Kd; //Diffuse map, kolla denna om objekt inte har texturer
		DirectX::XMFLOAT3 Ks; //Specular Color är färgen som objektet får när ljus reflekteras emot den
		DirectX::XMFLOAT3 Ke;//Emissive color is the color of a surface when it "lights up". For example a lightbulb.
	} material;
	
	ID3D11Buffer* meshVertexBuff;
	ID3D11Buffer* meshIndexBuff;
	int indexCount;

	ID3D11ShaderResourceView* textureShaderResource;
	ID3D11ShaderResourceView* normalShaderResource;

	int nrOfVertices;

	DirectX::XMFLOAT4X4 objMatrix;

	//change
	UINT32 vertexSize = sizeof(Vertex);

	ID3D11Buffer*   pixelShaderMaterialCB;
public:
	Model();
	~Model();


	bool Initialize(std::wstring _modelName, ID3D11Device* _device, std::vector<XMFLOAT3> *collidableGeometryPositions, std::vector<DWORD> *collidableGeometryIndices);
	void Shutdown();

	virtual void Update();
	void Render(ID3D11DeviceContext* _deviceContext);

	bool LoadObj(std::wstring _filename, ID3D11Device* _device, std::vector<XMFLOAT3> *collidableGeometryPositions, std::vector<DWORD> *collidableGeometryIndices);

	bool CreateShaders(ID3D11Device* _device);

	int GetIndexCount();

	DirectX::XMMATRIX GetObjMatrix();
	void SetObjMatrix(const DirectX::XMMATRIX& _newMatrix);
};
#endif