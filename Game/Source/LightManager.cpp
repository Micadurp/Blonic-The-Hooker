#include "LightManager.h"

LightManager::LightManager()
{
	lightBufferObj.lights = nullptr;
	lightBuffers = nullptr;
}

LightManager::~LightManager()
{
}

bool LightManager::Initialize(ID3D11Device* _device)
{
	HRESULT result;

	// Light shared information buffer struct
	lightSharedObj.ambient = { 0.2f, 0.2f, 0.2f, 0.2f };
	lightSharedObj.attenuation = { 0.0f, 0.2f, 0.0f };
	lightSharedObj.intensity = 100;
	lightSharedObj.range = 1000;

	
	// Lights
	lightBufferObj.lights = new LightStruct[lightCount];
	if (!lightBufferObj.lights)
	{
		return false;
	}

	XMFLOAT4 pos[] = { XMFLOAT4(25.0f, 25.0f, 5.0f, 1.0f), XMFLOAT4(-25.0f, 10.0f, -10.0f, 1.0f) };
	XMFLOAT4 diff[] = { XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) };

	for (int i = 0; i < lightCount; i++)
	{
		lightBufferObj.lights->position = pos[i];
		lightBufferObj.lights->diffuse = diff[i];
	}

	lightBuffers = new ID3D11Buffer*[2];


	D3D11_BUFFER_DESC lightBufferDesc;

	ZeroMemory(&lightBufferDesc, sizeof(D3D11_BUFFER_DESC));
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	lightBufferDesc.ByteWidth = sizeof(LightSharedStruct);
	result = _device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffers[0]);
	if (FAILED(result))
	{
		return false;
	}

	lightBufferDesc.ByteWidth = sizeof(LightStruct) * lightCount;
	result = _device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffers[1]);
	if (FAILED(result))
	{
		return false;
	}


#pragma region Temporary lights
	/*
	// Point lights
	lightColor.diff = new XMFLOAT4[lightCount];
	lightPos.pos = new XMFLOAT4[lightCount];

	lightColor.diff[0] = { 1.0f, 0.0f, 0.0f, 1.0f };
	lightColor.diff[1] = { 0.0f, 0.0f, 1.0f, 1.0f };

	lightPos.pos[0] = { 25.0f, 10.0f, 0.0f, 1.0f };
	lightPos.pos[1] = { 25.0f, 10.0f, -15.0f, 1.0f };

	D3D11_BUFFER_DESC lightBufferDesc;

	ZeroMemory(&lightBufferDesc, sizeof(D3D11_BUFFER_DESC));
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.ByteWidth = sizeof(XMFLOAT4) * lightCount;
	lightBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	result = _device->CreateBuffer(&lightBufferDesc, NULL, &lightColorBuffer);
	if (FAILED(result))
	{
		return false;
	}

	result = _device->CreateBuffer(&lightBufferDesc, NULL, &lightPosBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Directional light
	dirBufferObj.light.ambient = { 0.2f, 0.2f, 0.2f, 0.2f };
	dirBufferObj.light.diffuse = { 0.8f, 0.7f, 0.6f, 1.0f };
	dirBufferObj.light.dir = { 0.0f, 2.0f, 5.0f };
	dirBufferObj.light.intensity = 0.2f;


	ZeroMemory(&lightBufferDesc, sizeof(D3D11_BUFFER_DESC));
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.ByteWidth = sizeof(Directional);
	lightBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	
	result = _device->CreateBuffer(&lightBufferDesc, NULL, &directionalLightBuffer);
	*/
#pragma endregion

	return true;
}

void LightManager::ShutDown()
{
	if (lightBufferObj.lights)
	{
		delete lightBufferObj.lights;
	}

	if (lightBuffers)
	{
		lightBuffers[0]->Release();
		lightBuffers[1]->Release();
	}
}

void LightManager::Render(ID3D11DeviceContext* _deviceContext)
{
	_deviceContext->UpdateSubresource(lightBuffers[0], 0, NULL, &lightSharedObj, 0, 0);
	_deviceContext->UpdateSubresource(lightBuffers[1], 0, NULL, &lightBufferObj.lights, 0, 0);

	_deviceContext->PSSetConstantBuffers(0, 2, lightBuffers);


#pragma region Temporary lights
	/*
	// Point lights
	_deviceContext->UpdateSubresource(lightColorBuffer, 0, NULL, lightColor.diff, 0, 0);
	_deviceContext->UpdateSubresource(lightPosBuffer, 0, NULL, lightPos.pos, 0, 0);

	_deviceContext->PSSetConstantBuffers(0, 1, &lightColorBuffer);
	_deviceContext->PSSetConstantBuffers(1, 1, &lightPosBuffer);


	// Directional light
	_deviceContext->UpdateSubresource(directionalLightBuffer, 0, NULL, &dirBufferObj, 0, 0);
	_deviceContext->PSSetConstantBuffers(2, 1, &directionalLightBuffer);
	*/
#pragma endregion

}