#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <d3d11.h>
#include "LightClass.h"

class LightManager
{
private:
	const int lightCount = 2;

	struct LightStruct
	{
		XMFLOAT4 position;
		XMFLOAT4 diffuse;
		XMFLOAT4 ambient = { 0.2f, 0.2f, 0.2f, 0.2f };

		XMFLOAT3 attenuation = { 0.0f, 0.0f, 0.3f };
		float range = 1000.0f;

		float intensity = 200.0f;
		XMFLOAT3 pad;
	};

	struct LightBufferStruct
	{
		LightStruct* lights;
	} lightBufferObj;

	ID3D11Buffer* lightBuffer;

public:
	LightManager();
	~LightManager();

	bool Initialize(ID3D11Device* _device);

	void ShutDown();

	void Render(ID3D11DeviceContext* _deviceContext);
};

#endif