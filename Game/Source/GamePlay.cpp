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

	for (int n = 0; n < 1; n++)
	{
		models.push_back(new Model());
		models.at(n)->SetObjMatrix(DirectX::XMMatrixScaling(100, 0, 100) * DirectX::XMMatrixTranslation(0, -4, 0));
		models.at(n)->Initialize(L"ground", _device, &collidableGeometryPositions, &collidableGeometryIndices);
	} 	
	for (int n = models.size(); n < 10; n++)
	{
		models.push_back(new Model());
		models.at(n)->SetObjMatrix(DirectX::XMMatrixTranslation(rand() % 30 - 15, rand() % 30, rand() % 30 - 15));
		models.at(n)->Initialize(L"Cube", _device, &collidableGeometryPositions, &collidableGeometryIndices);
	}



	return true;
}


void GamePlay::Update(double time)
{
	player->ChangeHookState(models);
	player->Update(time, collidableGeometryPositions, collidableGeometryIndices);

}

void GamePlay::Render(Direct3D *_direct3D)
{

	for (int n = 0; n < models.size(); n++)
	{
		_direct3D->SetVertexCBuffer( models[n]->GetObjMatrix(), XMLoadFloat4x4(&player->GetViewMatrix()));
		models.at(n)->Render(_direct3D->GetDeviceContext());
	}
}
