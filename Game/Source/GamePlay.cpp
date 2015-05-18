#include "GamePlay.h"


GamePlay::GamePlay()
{
	player = nullptr;
	lightManager = nullptr;

}
GamePlay::~GamePlay()
{

}
void GamePlay::Shutdown()
{
	while (!models.empty())
	{
		models.back()->Shutdown();
		delete models.back();
		models.pop_back();
	}

	if (player)
	{
		player->Shutdown();
		delete player;
		player = 0;
	}

	if (lightManager)
	{
		lightManager->ShutDown();
		delete lightManager;
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


	for (int n = models.size(); n < 1; n++)
	{
		models.push_back(new Model());
		models.at(n)->SetObjMatrix(DirectX::XMMatrixScaling(1, 1, 1) * DirectX::XMMatrixTranslation(0, 0, 0));
<<<<<<< HEAD
		models.at(n)->Initialize(L"bana", _device, &collidableGeometryPositions, &collidableGeometryIndices);
=======
		models.at(n)->Initialize(L"hus_export", _device, &collidableGeometryPositions, &collidableGeometryIndices);
>>>>>>> cfa0d90e935e5a46995c22280adb6e9c9664960c
	} 	


	for (int n = models.size(); n < 2; n++)
	{
		models.push_back(new Model());
		models.at(n)->SetObjMatrix(DirectX::XMMatrixTranslation(0, 0, 0));
		models.at(n)->Initialize(L"kristall", _device, &collidableGeometryPositions, &collidableGeometryIndices, true);
	}	



	models.push_back(new SkyBox(_device));
	models.at(models.size() - 1)->SetObjMatrix(DirectX::XMMatrixScaling(1, 1, 1) * DirectX::XMMatrixTranslation(0, 0, 0));

#pragma region Create Scene Lights

	lightManager = new LightManager();
	if (!lightManager)
	{
		return false;
	}

	result = lightManager->Initialize(_device);
	if (!result)
	{
		return false;
	}

#pragma endregion



	return true;
}

int GamePlay::Update(double time)
{
	int state = -1;

	if (GameOver() == 2)
	{
		return 2;
	}

	state = player->Update(time, collidableGeometryPositions, collidableGeometryIndices, models);


	//lights[0].Position = player->GetPosition();

	for (size_t n = 0; n < models.size(); n++)
	{
		models[n]->Update(player);
	}


	return state;
}

void GamePlay::Render(Direct3D *_direct3D)
{
	_direct3D->Render(models, XMLoadFloat4x4(&player->GetViewMatrix()));

	_direct3D->SetCrosshairShaders();

	_direct3D->SetVertexCBuffer(XMLoadFloat4x4(&player->GetCrosshairMatrix()));

	player->Render(_direct3D->GetDeviceContext());

	// Deferred rendering
	lightManager->Render(_direct3D->GetDeviceContext());
	//_direct3D->SetPixelCBuffer(lightBuffer, sceneLightsObj, lightCount);
}

int GamePlay::GameOver()
{
	if (player->IsDead())
	{
		return 2;
	}

	return -1;
}
