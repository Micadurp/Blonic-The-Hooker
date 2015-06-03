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


bool GamePlay::Initialize(ID3D11Device* _device, HWND &_wndHandle, HINSTANCE &_hInstance, wstring _scene, wstring _sceneCollision, wstring _crystall, wstring _winPlane, int _currentLevel)
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

	models.push_back(new Model());
	models.back()->SetObjMatrix(DirectX::XMMatrixScaling(1, 1, 1) * DirectX::XMMatrixTranslation(0, 0, 0));
	models.back()->Initialize(_scene, _device);

	models.push_back(new Model());
	models.back()->SetObjMatrix(DirectX::XMMatrixTranslation(0, 0, 0));
	models.back()->Initialize(_sceneCollision, _device, &collidableGeometryPositions, &collidableGeometryIndices, false);


	models.push_back(new Model());
	models.back()->SetObjMatrix(DirectX::XMMatrixTranslation(0, 0, 0));
	models.back()->Initialize(_crystall, _device, &collidableGeometryPositions, &collidableGeometryIndices, true);


	models.push_back(new SkyBox());
	models.back()->Initialize(L"sphere", _device);
	models.back()->SetObjMatrix(DirectX::XMMatrixScaling(1, 1, 1) * DirectX::XMMatrixTranslation(0, 0, 0));

	models.push_back(new Model());
	models.back()->SetObjMatrix(DirectX::XMMatrixTranslation(0, 0, 0));
	models.back()->Initialize(_winPlane, _device, &collidableGeometryPositions, &collidableGeometryIndices, true);

#pragma region Create Scene Lights

	lightManager = new LightManager();
	if (!lightManager)
	{
		return false;
	}

	result = lightManager->Initialize(_device, _currentLevel);
	if (!result)
	{
		return false;
	}

	timer = new Timer();
	timer->Initialize();
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

	for (size_t n = 0; n < models.size(); n++)
	{
		models[n]->Update(player);
	}


	if (player->Win(models.back()))
	{
		state = 3;
	}

	timer->UpdateTimer();
	
	return state;
}

void GamePlay::Render(Direct3D *_direct3D, TextClass* _timer)
{

	_direct3D->Render(models.at(0), XMLoadFloat4x4(&player->GetViewMatrix()));
	_direct3D->Render(models.at(3), XMLoadFloat4x4(&player->GetViewMatrix()));

	_direct3D->SetCrosshairShaders();
	_direct3D->SetVertexCBuffer(XMLoadFloat4x4(&player->GetCrosshairMatrix()));

	player->Render(_direct3D);

	
	_direct3D->TurnOnAlphaBlending();
	_direct3D->SetCrosshairShaders();
	_timer->Update(timer->GetTimer());

	//_direct3D->SetShader();
	_timer->Render(_direct3D->GetDeviceContext());
	_direct3D->TurnOffAlphaBlending();

	// Set lights used in deferred rendering
	lightManager->Render(_direct3D->GetDeviceContext());
}

int GamePlay::GameOver()
{
	if (player->IsDead())
	{
		return 2;
	}

	return -1;
}
