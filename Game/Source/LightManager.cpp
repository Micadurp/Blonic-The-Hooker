#include "LightManager.h"

LightManager::LightManager()
{
	lightBufferObj.lights = nullptr;
	lightBuffer = nullptr;
}

LightManager::~LightManager()
{
}

bool LightManager::Initialize(ID3D11Device* _device)
{
	HRESULT result;


	// Directional light
	enviroLightObj.light.direction = { 0.0f, 1.0f, 2.0f };
	enviroLightObj.light.diffuse = { 0.8f, 0.8f, 0.7f, 1.0f };
	enviroLightObj.light.ambient = { 0.2f, 0.2f, 0.2f, 1.0f };


	// Point lights
	lightBufferObj.lights = new PointLight[lightCount];
	if (!lightBufferObj.lights)
	{
		return false;
	}
	
	XMFLOAT4 pos[] = { XMFLOAT4(25.0f, 25.0f, 5.0f, 1.0f), XMFLOAT4(-25.0f, 20.0f, -50.0f, 1.0f) };
	XMFLOAT4 diff[] = { XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) };

	for (int i = 0; i < lightCount; i++)
	{
		lightBufferObj.lights[i].position = pos[i];
		lightBufferObj.lights[i].diffuse = diff[i];
	}


	lightBuffer = new ID3D11Buffer*[2];
	

	D3D11_BUFFER_DESC lightBufferDesc;

	ZeroMemory(&lightBufferDesc, sizeof(D3D11_BUFFER_DESC));
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	lightBufferDesc.ByteWidth = sizeof(DirectionalLight);
	result = _device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer[0]);
	if (FAILED(result))
	{
		return false;
	}


	lightBufferDesc.ByteWidth = sizeof(PointLight) * lightCount;
	result = _device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer[1]);
	if (FAILED(result))
	{
		return false;
	}



	return true;
}

void LightManager::ShutDown()
{
	if (lightBufferObj.lights)
	{
		delete lightBufferObj.lights;
	}

	if (lightBuffer)
	{
		lightBuffer[0]->Release();
		lightBuffer[1]->Release();
	}
}

void LightManager::Render(ID3D11DeviceContext* _deviceContext)
{
	_deviceContext->UpdateSubresource(lightBuffer[0], 0, NULL, &enviroLightObj, 0, 0);
	_deviceContext->UpdateSubresource(lightBuffer[1], 0, NULL, lightBufferObj.lights, 0, 0);

	_deviceContext->PSSetConstantBuffers(0, 2, lightBuffer);
}