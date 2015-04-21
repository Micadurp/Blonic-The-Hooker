#ifndef SYSTEM_H
#define SYSTEM_H

///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include "GamePlay.h"
#include "Direct3D.h"
#include "Menu.h"

//move this
/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class System
{
private:
	int screenWidth;
	int screenHeight;

	LPCWSTR applicationName;
	HINSTANCE hinstance;
	HWND hwnd;

	enum GameState
	{
		gMenu,
		gGamePlay,
		gPause
	}
	gameState;
	
	Menu* menu;
	GamePlay* gamePlay;
	Direct3D* direct3D;

public:
	System();
	virtual ~System();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();


};

/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/////////////
// GLOBALS //
/////////////
static System* ApplicationHandle = 0;

#endif