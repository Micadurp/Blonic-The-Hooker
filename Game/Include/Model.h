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

#include "ObjLoader.h"

#include "Structs\ModelMaterial.h"
#include "Structs\Vertex.h"

class Player;

using namespace DirectX;

class Model
{
protected:	

	ID3D11Buffer* meshVertexBuff;
	ID3D11Buffer* meshIndexBuff;
	int indexCount;

	ID3D11ShaderResourceView* textureShaderResource = nullptr;
	ID3D11ShaderResourceView* normalShaderResource;

	std::vector<ModelMaterial> modelMats;

	int nrOfVertices;

	DirectX::XMFLOAT4X4 objMatrix;

	ID3D11Buffer* pixelShaderMaterialCB;

	Player* player;

	std::vector<XMFLOAT3> pickingPoints;
	std::vector<DWORD> pickingIndices;
public:
	Model();
	virtual ~Model();

	virtual bool Initialize(std::wstring _modelName, ID3D11Device* _device);
	bool Initialize(std::wstring _modelName, ID3D11Device* _device, std::vector<XMFLOAT3>& collidableGeometryPositions, std::vector<DWORD>& collidableGeometryIndices);
	bool Initialize(std::wstring _modelName, ID3D11Device* _device, std::vector<XMFLOAT3>& collidableGeometryPositions, std::vector<DWORD>& collidableGeometryIndices, bool pickable);
	
	virtual void Shutdown();

	virtual void Update(const Player& _player);
	virtual void Render(ID3D11DeviceContext* _deviceContext, ID3D11DepthStencilState * _depthState);
	virtual void Model::Render(ID3D11DeviceContext* _deviceContext);

	bool CreateShaders(ID3D11Device* _device);

	int GetIndexCount();

	DirectX::XMMATRIX GetObjMatrix();
	void SetObjMatrix(const DirectX::XMMATRIX& _newMatrix);


	std::vector<XMFLOAT3>* GetPickingPoints();
	std::vector<DWORD>* GetPickingIndicies();
};
#endif