#include "GamePlay.h"


GamePlay::GamePlay()
{

}
GamePlay::~GamePlay()
{

}
void GamePlay::Shutdown()
{ 
	while (models.size() != 0)
	{
		models.back()->Shutdown();
		delete models.back();
		models.pop_back();
	}
}


bool GamePlay::Initialize(ID3D11Device* _device, HWND &_wndHandle, HINSTANCE &_hInstance)
{
	bool result;

	player = new Player();
	if (!player)
	{
		return false;
	}

	result = player->Initialize(_wndHandle, _hInstance, _device);

	if (!result)
	{
		return false;
	}

	for (int n = 0; n < 1; n++)
	{
		models.push_back(new Model());
		models.at(n)->SetObjMatrix(DirectX::XMMatrixScaling(50, 1, 50) * DirectX::XMMatrixTranslation(0, -5, 0));
		models.at(n)->Initialize(L"ground", _device, &collidableGeometryPositions, &collidableGeometryIndices);
	} 	

	for (int n = models.size(); n < 100; n++)
	{
		models.push_back(new Model());
		models.at(n)->SetObjMatrix(DirectX::XMMatrixTranslation(rand() % 100 - 50, rand() % 100, rand() % 100 - 50));
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
	_direct3D->SetShader();

	for (int n = 0; n < models.size(); n++)
	{
		_direct3D->SetVertexCBuffer( models[n]->GetObjMatrix(), XMLoadFloat4x4(&player->GetViewMatrix()));
		models.at(n)->Render(_direct3D->GetDeviceContext());
	}

	_direct3D->SetCrosshairShaders();

	_direct3D->SetVertexCBuffer(XMLoadFloat4x4(&player->GetCrosshairMatrix()));
	player->Render(_direct3D->GetDeviceContext());
}
