#include "LightManager.h"

LightManager::LightManager()
{
}

LightManager::~LightManager()
{
}

bool LightManager::Initialize(ID3D11Device* _device)
{
	HRESULT result;

	// Directional light
	dirBufferObj.light.ambient = { 0.2f, 0.2f, 0.2f, 0.2f };
	dirBufferObj.light.diffuse = { 0.8f, 0.7f, 0.6f, 1.0f };
	dirBufferObj.light.dir = { 0.0f, 2.0f, 5.0f };
	dirBufferObj.light.intensity = 0.2f;


	D3D11_BUFFER_DESC lightBufferDesc;

	ZeroMemory(&lightBufferDesc, sizeof(D3D11_BUFFER_DESC));
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.ByteWidth = sizeof(Directional);
	lightBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	
	result = _device->CreateBuffer(&lightBufferDesc, NULL, &directionalLightBuffer);

	return true;
}

void LightManager::ShutDown()
{
	if (directionalLightBuffer)
	{
		directionalLightBuffer->Release();
		directionalLightBuffer = 0;
	}
}

void LightManager::Render(ID3D11DeviceContext* _deviceContext)
{
	_deviceContext->UpdateSubresource(directionalLightBuffer, 0, NULL, &dirBufferObj, 0, 0);
	_deviceContext->PSSetConstantBuffers(0, 1, &directionalLightBuffer);
}