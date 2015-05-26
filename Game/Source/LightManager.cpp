#include "LightManager.h"

LightManager::LightManager()
{
	srand((unsigned)time(0));

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
	enviroLightObj.light.diffuse = { 0.0f, 0.0f, 0.0f, 1.0f };
	enviroLightObj.light.ambient = { 0.2f, 0.2f, 0.2f, 1.0f };


	// Point lights
	lightBufferObj.lights = new PointLight[LIGHT_COUNT];
	if (!lightBufferObj.lights)
	{
		return false;
	}
	
	XMFLOAT4 pos01[] = { XMFLOAT4(15.0f, 30.0f, -11.0f, 1.0f), XMFLOAT4(-46.0f, 28.0f, 8.0f, 1.0f), XMFLOAT4(-20.0f, 31.0f, -3.0f, 1.0f), XMFLOAT4(37.0f, 11.0f, 9.0f, 1.0f) };
	XMFLOAT4 pos02[] = { XMFLOAT4(-1.0f, 46.0f, 7.5f, 1.0f), XMFLOAT4(-1.3f, 26.0f, 65.0f, 1.0f), XMFLOAT4(-3.70f, 65.20f, 70.5f, 1.0f), XMFLOAT4(-113.8f, 26.7f, 81.5f, 1.0f), XMFLOAT4(-121.3f, 64.7f, 61.0f, 1.0f), XMFLOAT4(-117.8f, 51.7f, 37.2f, 1.0f), XMFLOAT4(-111.8f, 50.7f, -12.5f, 1.0f), XMFLOAT4(-175.8f, 57.7f, -10.5f, 1.0f), XMFLOAT4(-227.8f, 69.7f, 77.5f, 1.0f), XMFLOAT4(-233.8f, 73.7f, 36.5f, 1.0f), XMFLOAT4(-266.4f, 56.7f, 23.5f, 1.0f), XMFLOAT4(-266.4f, 64.7f, 68.5f, 1.0f) };
	XMFLOAT4 diff = XMFLOAT4(0.2f, 0.4f, 1.0f, 1.0f);

	for (int i = 0; i < LIGHT_COUNT; i++)
	{
		lightBufferObj.lights[i].position = pos02[i];
		lightBufferObj.lights[i].diffuse = diff;
		lightBufferObj.lights[i].intensity = 10;
		lightBufferObj.lights[i].attenuation = { 0.0f, 0.0f, 0.6f };
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


	lightBufferDesc.ByteWidth = sizeof(PointLight) * LIGHT_COUNT;
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

void LightManager::SetLightPosition(XMFLOAT4 _pos)
{
	lightBufferObj.lights[0].position = _pos;
}

void LightManager::Render(ID3D11DeviceContext* _deviceContext)
{
	_deviceContext->UpdateSubresource(lightBuffer[0], 0, NULL, &enviroLightObj, 0, 0);
	_deviceContext->UpdateSubresource(lightBuffer[1], 0, NULL, lightBufferObj.lights, 0, 0);

	_deviceContext->PSSetConstantBuffers(0, 2, lightBuffer);
}