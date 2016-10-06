#include "Model.h"
#include "Player.h"

Model::Model()
{
	DirectX::XMStoreFloat4x4(&objMatrix, DirectX::XMMatrixIdentity());
	normalShaderResource = nullptr;
	meshVertexBuff = nullptr;
	meshIndexBuff = nullptr;
	pixelShaderMaterialCB = nullptr;
	indexCount = 0;
}

Model::~Model()
{

}

bool Model::Initialize(std::wstring _modelName,
	ID3D11Device* _device)
{
	bool result;

	result = ObjLoader::LoadObj(
		_modelName,
		_device,
		meshVertexBuff,
		meshIndexBuff,
		indexCount,
		pickingPoints,
		pickingIndices,
		objMatrix,
		modelMats
	);
	if (!result)
	{
		return false;
	}

	result = CreateShaders(_device);
	if (!result)
	{
		return false;
	}
	return true;
}

bool Model::Initialize(std::wstring _modelName,
	ID3D11Device* _device,
	std::vector<XMFLOAT3>& _collidableGeometryPositions,
	std::vector<uint32_t>& _collidableGeometryIndices
)
{
	bool result;

	result = ObjLoader::LoadObj(
		_modelName,
		_device,
		meshVertexBuff,
		meshIndexBuff,
		indexCount,
		pickingPoints,
		pickingIndices,
		objMatrix,
		modelMats,
		&_collidableGeometryPositions,
		&_collidableGeometryIndices
	);
	if (!result)
	{
		return false;
	}

	result = CreateShaders(_device);
	if (!result)
	{
		return false;
	}
	return true;
}
bool Model::Initialize(std::wstring _modelName, 
	ID3D11Device* _device, 
	std::vector<XMFLOAT3>& _collidableGeometryPositions, 
	std::vector<uint32_t>& _collidableGeometryIndices,
	bool _pickable)
{
	bool result;

	result = ObjLoader::LoadObj(
		_modelName, 
		_device,
		meshVertexBuff,
		meshIndexBuff,
		indexCount,
		pickingPoints,
		pickingIndices,
		objMatrix,
		modelMats,
		&_collidableGeometryPositions, 
		&_collidableGeometryIndices, 
		_pickable
	);
	if (!result)
	{
		return false;
	}

	result = CreateShaders(_device);
	if (!result)
	{
		return false;
	}
	return true;
}

void Model::Shutdown()
{

	// Release the index buffer.
	if (meshIndexBuff)
	{
		meshIndexBuff->Release();
		meshIndexBuff = 0;
	}

	// Release the vertex buffer.
	if (meshVertexBuff)
	{
		meshVertexBuff->Release();
		meshVertexBuff = 0;
	}

	for (size_t i = 0; i < modelMats.size(); i++)
	{
		if (modelMats.at(i).texture)
		{
			modelMats.at(i).texture->Release();
			modelMats.at(i).texture = 0;
		}
	}

	if (normalShaderResource)
	{
		normalShaderResource->Release();
		normalShaderResource = 0;
	}

	if (pixelShaderMaterialCB)
	{
		pixelShaderMaterialCB->Release();
		pixelShaderMaterialCB = 0;
	}
}

void Model::Update(const Player& _player)
{
	return;
}

void Model::Render(ID3D11DeviceContext* _deviceContext, ID3D11DepthStencilState * _depthState)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	_deviceContext->IASetVertexBuffers(0, 1, &meshVertexBuff, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	_deviceContext->IASetIndexBuffer(meshIndexBuff, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (pixelShaderMaterialCB)
		_deviceContext->PSSetConstantBuffers(0, 1, &pixelShaderMaterialCB);

	int indexStart = 0;

	for (size_t n = 0; n < modelMats.size(); n++)
	{
		_deviceContext->PSSetShaderResources(0, 1, &modelMats.at(n).texture);

		_deviceContext->DrawIndexed(modelMats.at(n).indexAmount, indexStart, 0);
		indexStart += modelMats.at(n).indexAmount;
	}

	return;
}

void Model::Render(ID3D11DeviceContext* _deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	_deviceContext->IASetVertexBuffers(0, 1, &meshVertexBuff, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	_deviceContext->IASetIndexBuffer(meshIndexBuff, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (pixelShaderMaterialCB)
		_deviceContext->PSSetConstantBuffers(0, 1, &pixelShaderMaterialCB);

	int indexStart = 0;

	for (size_t n = 0; n < modelMats.size(); n++)
	{
		_deviceContext->PSSetShaderResources(0, 1, &modelMats.at(n).texture);

		_deviceContext->DrawIndexed(modelMats.at(n).indexAmount, indexStart, 0);
		indexStart += modelMats.at(n).indexAmount;
	}

	return;
}

bool Model::CreateShaders(ID3D11Device* _device)
{
	D3D11_BUFFER_DESC materialDesc;
	D3D11_SUBRESOURCE_DATA materialSubResource;

	memset(&materialDesc, 0, sizeof(materialDesc));

	materialSubResource.pSysMem = &modelMats.front().material;
	materialSubResource.SysMemPitch = 0;
	materialSubResource.SysMemSlicePitch = 0;


	materialDesc.ByteWidth = sizeof(Material);
	materialDesc.Usage = D3D11_USAGE_DEFAULT;
	materialDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialDesc.MiscFlags = 0;
	materialDesc.StructureByteStride = 0;


	HRESULT hr = _device->CreateBuffer(&materialDesc, &materialSubResource, &pixelShaderMaterialCB);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

int Model::GetIndexCount()
{
	return indexCount;
}

DirectX::XMMATRIX Model::GetObjMatrix()
{
	return DirectX::XMLoadFloat4x4(&objMatrix);
}

void Model::SetObjMatrix(const DirectX::XMMATRIX& _newMatrix)
{
	DirectX::XMStoreFloat4x4(&objMatrix, _newMatrix);
}

std::vector<XMFLOAT3>* Model::GetPickingPoints()
{
	return &pickingPoints;
}

std::vector<uint32_t>* Model::GetPickingIndicies()
{

	return &pickingIndices;
}