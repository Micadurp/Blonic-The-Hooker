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

	// Lights
	lightBufferObj.lights = new LightStruct[lightCount];
	if (!lightBufferObj.lights)
	{
		return false;
	}

	//lightBufferObj.lights[0].position = { 20.0f, 25.0f, 5.0f, 1.0f };
	//lightBufferObj.lights[0].diffuse = { 1.0f, 0.0f, 0.0f, 1.0f };

	
	XMFLOAT4 pos[] = { XMFLOAT4(25.0f, 25.0f, 5.0f, 1.0f), XMFLOAT4(-25.0f, 20.0f, -50.0f, 1.0f) };
	XMFLOAT4 diff[] = { XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) };

	for (int i = 0; i < lightCount; i++)
	{
		lightBufferObj.lights[i].position = pos[i];
		lightBufferObj.lights[i].diffuse = diff[i];
	}
	

	D3D11_BUFFER_DESC lightBufferDesc;

	ZeroMemory(&lightBufferDesc, sizeof(D3D11_BUFFER_DESC));
	lightBufferDesc.ByteWidth = sizeof(LightStruct) * lightCount;
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	result = _device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
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
		lightBuffer->Release();
	}
}

void LightManager::Render(ID3D11DeviceContext* _deviceContext)
{
	_deviceContext->UpdateSubresource(lightBuffer, 0, NULL, lightBufferObj.lights, 0, 0);

	_deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);
}