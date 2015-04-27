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

	std::vector<XMFLOAT3> collidableGeometryPositions;
	std::vector<DWORD> collidableGeometryIndices;
public:
	GamePlay();
	virtual ~GamePlay();

	bool Initialize(ID3D11Device* _device, HWND &wndHandle, HINSTANCE &hInstance);
	void Shutdown();

	void Update(double time);
	void Render(ID3D11DeviceContext* _deviceContext, RenderManager* _renderer, const DirectX::XMMATRIX &_projectionMatrix);

};

#endif