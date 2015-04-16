#include "GamePlay.h"


GamePlay::GamePlay()
{

}
GamePlay::~GamePlay()
{

}

void GamePlay::Update()
{

}

void GamePlay::Render(Direct3D*& direct3D)
{
	// Clear the buffers to begin the scene.
	direct3D->BeginScene(0.0f, 0.0f, 0.5f, 1.0f);


	direct3D->EndScene();
}