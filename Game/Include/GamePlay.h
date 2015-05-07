#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <vector>

#include "Direct3D.h"
#include "Model.h"
#include "Player.h"
#include "LightStructure.h"


class GamePlay
{
private:
	const int lightCount = 1;

	LightPosColor lightsObj;
	LightSharedInfo lightSharedObj;

	ID3D11Buffer** lightBuffer;

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


	void Render(Direct3D * _direct3D);

	int Update(double _time);
};

#endif