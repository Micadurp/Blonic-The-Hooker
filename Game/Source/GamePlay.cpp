#include "GamePlay.h"


GamePlay::GamePlay()
{

}
GamePlay::~GamePlay()
{

}
void GamePlay::Shutdown()
{ 
	/*if (cube)
	{
	cube->Shutdown();
	delete cube;
	cube = 0;
	}*/
}


bool GamePlay::Initialize(ID3D11Device* _device, HWND &_wndHandle, HINSTANCE &_hInstance)
{
	bool result;

	player = new Player();
	if (!player)
	{
		return false;
	}

	result = player->Initialize(_wndHandle, _hInstance);

	if (!result)
	{
		return false;
	}


	models = new Model*[]
	{
		new Model(),
		new Model(),
		new Model(),
		new Model(),
		new Model(),
		new Model(),
		new Model()
	};

	for (int n = 0; n < 1; n++)
	{
		models[n]->SetObjMatrix(DirectX::XMMatrixScaling(100, 0, 100) * DirectX::XMMatrixTranslation(0, -4, 0));
		models[n]->Initialize(L"ground", _device, &collidableGeometryPositions, &collidableGeometryIndices);
	} 	
	for (int n = 1; n < 6; n++)
	{
		models[n]->SetObjMatrix(DirectX::XMMatrixTranslation(rand() % 20 - 10, rand() % 20, rand() % 20 - 10));
		models[n]->Initialize(L"Cube", _device, &collidableGeometryPositions, &collidableGeometryIndices);
	}



	return true;
}

void GamePlay::Update(double time)
{
	player->Update(time, collidableGeometryPositions, collidableGeometryIndices);
}

void GamePlay::Render(ID3D11DeviceContext* _deviceContext, RenderManager* _renderer, const DirectX::XMMATRIX &_projectionMatrix)
{
	for (int n = 0; n < 6; n++)
	{
		_renderer->SetVertexCBuffer(_deviceContext, models[n]->GetObjMatrix(), XMLoadFloat4x4(&player->GetViewMatrix()), _projectionMatrix);
		models[n]->Render(_deviceContext);
	}
}

