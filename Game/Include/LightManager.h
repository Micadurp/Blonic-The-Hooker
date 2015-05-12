#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <d3d11.h>
#include "LightClass.h"

class LightManager
{
private:
	const int lightCount = 2;

	struct Directional
	{
		Directional()
		{
			ZeroMemory(this, sizeof(Directional));
		}
		XMFLOAT3 dir;
		float intensity;

		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
	} dirLight;

	struct lightBufferStruct
	{
		Directional light;
	} dirBufferObj;

	struct LightColorStruct
	{
		XMFLOAT4* diff;
	} lightColor;

	struct LightPositionStruct
	{
		XMFLOAT4* pos;
	} lightPos;
	
	LightClass* lights;

	ID3D11Buffer* directionalLightBuffer;

	ID3D11Buffer* lightColorBuffer;
	ID3D11Buffer* lightPosBuffer;

public:
	LightManager();
	~LightManager();

	bool Initialize(ID3D11Device* _device);

	void ShutDown();

	void Render(ID3D11DeviceContext* _deviceContext);
};

#endif