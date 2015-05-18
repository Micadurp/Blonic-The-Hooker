#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <vector>

#include "Direct3D.h"
#include "Model.h"
#include "Player.h"
#include "LightManager.h"

#include "SkyBox.h"

class GamePlay
{
private:
	LightManager* lightManager;

	RenderManager* renderer;

	std::vector< Model*> models;

	Player* player;

	vector<XMFLOAT3> collidableGeometryPositions;
	vector<DWORD> collidableGeometryIndices;
	
public:
	GamePlay();
	virtual ~GamePlay();

	void Render(ID3D11DeviceContext* _deviceContext, RenderManager* _renderer, const DirectX::XMMATRIX &_projectionMatrix);

	bool Initialize(ID3D11Device* _device, HWND &_wndHandle, HINSTANCE &_hInstance);
	void Shutdown();

	int Update(double _time);

	void Render(Direct3D * _direct3D);

	int GameOver();

};

#endif