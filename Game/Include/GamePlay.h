#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "Direct3D.h"
#include "Model.h"
#include "RenderManager.h"
#include "Camera.h"

class GamePlay
{
private:
	RenderManager* renderer;
	Model* cube;
	Camera* camera;
public:
	GamePlay();
	virtual ~GamePlay();

	bool Initialize(ID3D11Device* _device);
	void Shutdown();

	void Update();
	void Render(ID3D11DeviceContext* _deviceContext, const DirectX::XMMATRIX &_projectionMatrix);
};

#endif