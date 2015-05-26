#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <d3d11.h>
#include <ctime>
#include "LightStructure.h"

using namespace std;

class LightManager
{
private:
	const int LIGHT_COUNT = 12;

	struct EnviromentLightBuffer
	{
		DirectionalLight light;
	} enviroLightObj;

	struct LightBufferStruct
	{
		PointLight* lights;
	} lightBufferObj;

	ID3D11Buffer** lightBuffer;

public:
	LightManager();
	~LightManager();

	bool Initialize(ID3D11Device* _device);

	void SetLightPosition(XMFLOAT4 _pos);
	void ShutDown();

	void Render(ID3D11DeviceContext* _deviceContext);
};

#endif