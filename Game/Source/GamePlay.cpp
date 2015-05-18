#include "GamePlay.h"


GamePlay::GamePlay()
{
	player = nullptr;
	lightManager = nullptr;
	sceneLightsObj= new LightInfo[lightCount];
	lightBuffer = nullptr;

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

	if (sceneLightsObj)
	{
		//delete sceneLightsObj;
	}

	if (lightBuffer)
	{
		lightBuffer->Release();
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
		models.at(n)->Initialize(L"hus_Export", _device, &collidableGeometryPositions, &collidableGeometryIndices);
	} 	


	for (int n = models.size(); n < 2; n++)
	{
		models.push_back(new Model());
		models.at(n)->SetObjMatrix(DirectX::XMMatrixTranslation(0, 0, 0));
		models.at(n)->Initialize(L"kristall_export", _device, &collidableGeometryPositions, &collidableGeometryIndices, true);
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

	sceneLightsObj[0].Position = { 25.0f, 25.0f, 5.0f, 1.0f };
	sceneLightsObj[0].Color = { 1.0f, 1.0f, 0.0f, 1.0f };

#pragma endregion


#pragma region Create Light buffers
	HRESULT hr;
	
	D3D11_BUFFER_DESC lightBufferDesc;

	memset(&lightBufferDesc, 0, sizeof(lightBufferDesc));
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.ByteWidth = sizeof(LightInfo) * lightCount;
	lightBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	hr = _device->CreateBuffer(&lightBufferDesc, nullptr, &lightBuffer);

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
