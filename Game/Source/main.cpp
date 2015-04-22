#include "System.h"

int WINAPI wWinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPWSTR _lpCmdLine, int _nCmdShow)
{
	System* theSystem;
	bool result;


	// Create the system object.
	theSystem = new System;
	if (!theSystem)
	{
		return 0;
	}

	// Initialize and run the system object.
	result = theSystem->Initialize();
	if (result)
	{
		theSystem->Run();
	}

	// Shutdown and release the system object.
	theSystem->Shutdown();
	delete theSystem;
	theSystem = 0;

	return 0;
}