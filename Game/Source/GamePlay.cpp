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

bool GamePlay::Initialize(ID3D11Device* _device)
{
	bool result;

	


		models.push_back(new Model());


		for (int n = 0; n < models.size(); n++)
			models[n]->Initialize(L"Cube", _device);



	return true;
}

void GamePlay::Update()
{
	
}

void GamePlay::Render(ID3D11DeviceContext* _deviceContext, const DirectX::XMMATRIX &_projectionMatrix)
{

	for (int n = 0; n < models.size(); n++)
		models[n]->Render(_deviceContext);

}