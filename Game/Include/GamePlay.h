#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "Direct3D.h"
#include "Model.h"
#include "RenderManager.h"
#include "Player.h"

class GamePlay
{
private:
	RenderManager* renderer;
	vector<Model*> models;
	Player* player;

	vector<XMFLOAT3> collidableGeometryPositions;
	vector<DWORD> collidableGeometryIndices;

public:
	GamePlay();
	virtual ~GamePlay();

	void Render(ID3D11DeviceContext* _deviceContext, RenderManager* _renderer, const DirectX::XMMATRIX &_projectionMatrix);

	bool Initialize(ID3D11Device* _device, HWND &_wndHandle, HINSTANCE &_hInstance);
	void Shutdown();

	void Update(double _time);
};

#endif