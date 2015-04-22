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

	/*if (cube)
	{
		cube->Shutdown();
		delete cube;
		cube = 0;
	}*/
}

bool GamePlay::Initialize(ID3D11Device* _device)
{
	bool result;

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

	models = new Model*[]
	{
		new Model(),
		new Model(),
		new Model(),
	};

	for (int n = 0; n < sizeof(models) -1; n++)
		models[n]->Initialize(L"Cube", _device);



	return true;
}

void GamePlay::Update()
{
	camera->Update();
}

void GamePlay::Render(ID3D11DeviceContext* _deviceContext, const DirectX::XMMATRIX &_projectionMatrix)
{
	for (int n = 0; n < sizeof(models)-1; n++)
	{
		renderer->SetShader(_deviceContext, XMMatrixTranslation(n * 1.3f, 0.0f, 5.0f), camera->GetViewMatrix(), _projectionMatrix);
		models[n]->Render(_deviceContext);
	}

}