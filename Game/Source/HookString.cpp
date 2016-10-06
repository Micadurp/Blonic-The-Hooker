#include"HookString.h"

HookString::HookString()
{
	hookStringGSCB = nullptr;
}

HookString::~HookString()
{

}

bool HookString::Initialize(ID3D11Device * _device)
{
	HRESULT result;

	isActive = false;
	
	//Model::Initialize(L"starParticle", _device);

	Vertex triangleVertices[1];
	triangleVertices[0] = Vertex(0, 0, -2,0,0,0,0,0);

	D3D11_BUFFER_DESC vertexBufDesc;
	memset(&vertexBufDesc, 0, sizeof(vertexBufDesc));
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.ByteWidth = sizeof(triangleVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;
	_device->CreateBuffer(&vertexBufDesc, &data, &meshVertexBuff);

	D3D11_BUFFER_DESC hookStringCBDesc;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	hookStringCBDesc.Usage = D3D11_USAGE_DYNAMIC;
	hookStringCBDesc.ByteWidth = sizeof(GeometryShaderBuffer);
	hookStringCBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hookStringCBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hookStringCBDesc.MiscFlags = 0;
	hookStringCBDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class
	result = _device->CreateBuffer(&hookStringCBDesc, NULL, &hookStringGSCB);
	if (FAILED(result))
	{
		return false;
	}
	XMStoreFloat4x4(&objMatrix,XMMatrixIdentity());


	result = DirectX::CreateDDSTextureFromFile(_device, L"Rope.dds", NULL, &textureShaderResource);

	

	if (result)
		true;
	
	return true;
}

void HookString::Shutdown()
{
	if (hookStringGSCB)
	{
		delete hookStringGSCB;
		hookStringGSCB = 0;
	}
}

void HookString::Update(Direct3D* _direct3D,XMMATRIX * viewMatrix, XMVECTOR * _playerPos, XMVECTOR * _target)
{
//	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	GeometryShaderBuffer* dataPtr;

	
	_direct3D->GetDeviceContext()->Map(hookStringGSCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// Get a pointer to the data in the constant buffer.
	dataPtr = (GeometryShaderBuffer*)mappedResource.pData;

	// Copy the matrices into the constant buffer.

	dataPtr->viewMatrix = XMMatrixTranspose(*viewMatrix * _direct3D->GetProjectionMatrix());
	dataPtr->targetPos = *_target;
	dataPtr->playerPos = *_playerPos;

	// Unlock the constant buffer.
	_direct3D->GetDeviceContext()->Unmap(hookStringGSCB, 0);

	// Now set the constant buffer in the vertex shader with the updated values.
	_direct3D->GetDeviceContext()->GSSetConstantBuffers(0, 1, &hookStringGSCB);
}

void HookString::Render(Direct3D* _direct3D)
{
	_direct3D->SetHookStringShaders();
	
	//Model::Render(_direct3D->GetDeviceContext());

	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	_direct3D->GetDeviceContext()->IASetVertexBuffers(0, 1, &meshVertexBuff, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	_direct3D->GetDeviceContext()->IASetIndexBuffer(meshIndexBuff, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	_direct3D->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);


	_direct3D->GetDeviceContext()->PSSetShaderResources(0, 1, &textureShaderResource);

	if (pixelShaderMaterialCB)
		_direct3D->GetDeviceContext()->PSSetConstantBuffers(0, 1, &pixelShaderMaterialCB);

	_direct3D->GetDeviceContext()->Draw(1, 0);

	return;
}

bool HookString::GetActive()
{
	return isActive;
}

void HookString::SetActive(bool _isActive)
{
	isActive = _isActive;
}