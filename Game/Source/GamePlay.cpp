#include "GamePlay.h"


GamePlay::GamePlay()
{

}
GamePlay::~GamePlay()
{

}
void GamePlay::Shutdown()
{
	if (renderer)
	{
		renderer->Shutdown();
		delete renderer;
		renderer = 0;
	}

	if (cube)
	{
		cube->Shutdown();
		delete cube;
		cube = 0;
	}
}

bool GamePlay::Initialize(ID3D11Device* device)
{

	renderer = new RenderManager();
	if (!renderer)
	{
		return false;
	}
	renderer->Initilize(device);

	camera = new Camera();
	if (!camera)
	{
		return false;
	}

	cube = new Model();
	if (!cube)
	{
		return false;
	}	

	cube->Initialize(L"cube.obj", device);

	return true;
}

void GamePlay::Update()
{
	camera->Render();
}

void GamePlay::Render(ID3D11DeviceContext* deviceContext, const DirectX::XMMATRIX &projectionMatrix)
{
	renderer->SetShader(deviceContext, XMMatrixTranslation(0.0f, 0.0f, 5.0f), camera->GetViewMatrix(), projectionMatrix);
	cube->Render(deviceContext);
	
}