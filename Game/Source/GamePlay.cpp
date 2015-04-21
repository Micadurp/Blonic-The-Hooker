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

bool GamePlay::Initialize(ID3D11Device* _device)
{

	renderer = new RenderManager();
	if (!renderer)
	{
		return false;
	}
	renderer->Initilize(_device);

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

	cube->Initialize(L"cube.obj", _device);

	return true;
}

void GamePlay::Update()
{
	camera->Render();
}

void GamePlay::Render(ID3D11DeviceContext* _deviceContext, const DirectX::XMMATRIX &_projectionMatrix)
{
	renderer->SetShader(_deviceContext, XMMatrixTranslation(0.0f, 0.0f, 5.0f), camera->GetViewMatrix(), _projectionMatrix);
	cube->Render(_deviceContext);
	
}