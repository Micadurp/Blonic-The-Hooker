#include "GamePlay.h"


GamePlay::GamePlay()
{
	lights = new LightInfo[lightCount];

	// Slot 0 being shared light properties
	// Slot 1 being light position and color array
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
		models.at(n)->SetObjMatrix(DirectX::XMMatrixScaling(1, 1, 1) * DirectX::XMMatrixTranslation(0, 0, 0));
		models.at(n)->Initialize(L"bana", _device, &collidableGeometryPositions, &collidableGeometryIndices);
	} 	


	for (int n = models.size(); n < 2; n++)
	{
		models.push_back(new Model());
		models.at(n)->SetObjMatrix(DirectX::XMMatrixTranslation(0, 0, 0));
		models.at(n)->Initialize(L"kristall", _device, &collidableGeometryPositions, &collidableGeometryIndices, true);
	}

#pragma region Create Scene Lights
	lights[0].Position = { 50.0f, 20.0f, 100.0f, 1.0f };
	lights[0].Color = { 1.0f, 0.0f, 0.0f, 1.0f };

#pragma endregion


#pragma region Create Light buffers
	HRESULT hr;
	
	D3D11_BUFFER_DESC lightBufferDesc;

	memset(&lightBufferDesc, 0, sizeof(lightBufferDesc));
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.ByteWidth = sizeof(LightInfo);
	lightBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hr = _device->CreateBuffer(&lightBufferDesc, nullptr, &lightBuffer);

#pragma endregion

	models.push_back(new SkyBox(_device));
	models.at(models.size()-1)->SetObjMatrix(DirectX::XMMatrixScaling(1, 1, 1) * DirectX::XMMatrixTranslation(0, 0, 7));

	return true;
}

int GamePlay::Update(double time)
{
	int state = -1;

	player->ChangeHookState(models);
	state = player->Update(time, collidableGeometryPositions, collidableGeometryIndices);


	lights[0].Position = player->GetPosition();

	for (int n = 0; n < models.size(); n++)
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
	_direct3D->SetPixelCBuffer(lightBuffer, lights[0]);
}
