#include"HookString.h"

HookString::HookString()
{
	point = new XMVECTOR();

	hookStringGSCB = nullptr;
}

HookString::~HookString()
{

}

bool HookString::Initialize(ID3D11Device *_device )
{
	HRESULT result;

	isActive = false;

	Model::Initialize(L"Cuck",_device);

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
	

	return true;
}

void HookString::Update(ID3D11DeviceContext* _deviceContext, XMVECTOR * _playerPos, XMVECTOR * _target)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	GeometryShaderBuffer* dataPtr;

	_deviceContext->Map(hookStringGSCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);


	// Get a pointer to the data in the constant buffer.
	dataPtr = (GeometryShaderBuffer*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->intNrOfParticles = 1;
	dataPtr->targetPos = *_target;
	dataPtr->playerPos = *_playerPos;

	// Unlock the constant buffer.
	_deviceContext->Unmap(hookStringGSCB, 0);

	// Now set the constant buffer in the vertex shader with the updated values.
	_deviceContext->GSSetConstantBuffers(0, 1, &hookStringGSCB);
}

void HookString::Render(Direct3D* _direct3D)
{
	if (isActive)
		Model::Render(_direct3D->GetDeviceContext());
}

bool HookString::GetActive()
{
	return isActive;
}

void HookString::SetActive(bool _isActive)
{
	isActive = _isActive;
}