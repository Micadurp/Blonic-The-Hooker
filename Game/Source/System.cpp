#include "System.h"

System::System()
{
}

System::~System()
{
}

bool System::Initialize()
{
	//Window Handeling////
	int screenWidth, screenHeight;
	bool result;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	///////////////

	gameState = GameState::gGamePlay;

	direct3D = new Direct3D();
	if (!direct3D)
	{
		return false;
	}

	result = direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	menu = new Menu();
	if (!menu)
	{
		return false;
	}

	gamePlay = new GamePlay();
	if (!gamePlay)
	{
		return false;
	}
	result = gamePlay->Initialize(direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Gameplay", L"Error", MB_OK);
		return false;
	}

	input = new PlayerInputs();
	if (!input)
	{
		return false;
	}
	result = input->Initialize(hwnd, hinstance);
	if (!result)
	{
		return false;
	}


	return true;
}

void System::Shutdown()
{
	if (gamePlay)
	{
		gamePlay->Shutdown();
		delete gamePlay;
		gamePlay = 0;
	}

	// Shutdown the window.
	ShutdownWindows();
}

void System::Run()
{
	MSG msg;
	bool done, result;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}

		// Check if the user pressed escape and wants to quit.
		/*if (input->IsEscapePressed() == true)
		{
			done = true;
		}*/
	}

	return;

}

bool System::Frame()
{

#pragma region Update
	switch (gameState)
	{
	case GameState::gGamePlay:
		gamePlay->Update();
		break;

	case GameState::gMenu:
		menu->Update();
		break;

	case GameState::gPause:

		break;
	}
#pragma endregion

#pragma region Draw

	direct3D->BeginScene(0.0f, 0.0f, 0.5f, 1.0f);

	switch (gameState)
	{
	case GameState::gGamePlay:
		gamePlay->Render(direct3D->GetDeviceContext(), direct3D->GetProjectionMatrix());


		break;

	case GameState::gMenu:
		menu->Render();
		break;

	case GameState::gPause:

		break;
	}

	direct3D->EndScene();
#pragma endregion

	return true;
}

LRESULT CALLBACK System::MessageHandler(HWND _hwnd, UINT _umsg, WPARAM _wparam, LPARAM _lparam)
{
	return DefWindowProc(_hwnd, _umsg, _wparam, _lparam);
}

void System::InitializeWindows(int& _screenWidth, int& _screenHeight)
{
	WNDCLASSEX wcex;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get an external pointer to this object.
	ApplicationHandle = this;

	// Get the instance of this application.
	hinstance = GetModuleHandle(NULL);

	// Give the application a name.

	applicationName = L"Blonic Adventure 2: Get Hooked";


	// Setup the windows class with default settings.
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hinstance;
	wcex.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wcex.hIconSm = wcex.hIcon;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = applicationName;
	wcex.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wcex);

	// Determine the resolution of the clients desktop screen.
	_screenWidth = GetSystemMetrics(SM_CXSCREEN);
	_screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)_screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)_screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		_screenWidth = 800;
		_screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - _screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - _screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		applicationName,
		applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU,
		posX,
		posY,
		_screenWidth,
		_screenHeight,
		nullptr,
		nullptr,
		hinstance,
		nullptr);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);

	return;
}

void System::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(hwnd);
	hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(applicationName, hinstance);
	hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam)
{
	switch (_message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CLOSE:
		{
						 PostQuitMessage(0);
						 return 0;
		}
	}

	return DefWindowProc(_hWnd, _message, _wParam, _lParam);
}