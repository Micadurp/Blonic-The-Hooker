#include "System.h"

System::System()
{
	currentLevel = 2;

	gamePlay = nullptr;

	LevelInfo level;
	level.scene = L"hus01_export";
	level.collision = L"hus01_collision";
	level.kristall = L"kristall01_collision";
	level.winPlane = L"hus01_winning";
	levels.push_back(level);

	level.scene = L"hus02_export";
	level.collision = L"hus02_collision";
	level.kristall = L"kristall02_collision";
	level.winPlane = L"hus02_winning";
	levels.push_back(level);

	level.scene = L"hus04_export";
	level.collision = L"hus04_collision";
	level.kristall = L"crystal04_collision";
	level.winPlane = L"hus04_winning";
	levels.push_back(level);
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

	countsPerSecond = 0.0;
	counterStart = 0;
	frameCount = 0;
	fps = 0;
	frameTimeOld = 0;
	frameTime = 0.0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	///////////////

	gameState = GameState::gMenu;

	direct3D = new Direct3D();
	if (!direct3D)
	{
		return false;
	}

	timer = new TextClass();
	if (!timer)
	{
		return false;
	}

	result = direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR, timer);
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

	wstring menuButtons[] = { L"menuBtn_newGame", L"menuBtn_Quit" };
	result = menu->Initialize(direct3D->GetDevice(), hwnd, hinstance, L"menuBgrd_menu", menuButtons, (float)screenWidth, (float)screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	pauseMenu = new Menu();
	if (!pauseMenu)
	{
		return false;
	}

	wstring pauseMenuButtons[] = { L"menuBtn_resume", L"menuBtn_Quit" };
	result = pauseMenu->Initialize(direct3D->GetDevice(), hwnd, hinstance, L"menuBgrd_pause", pauseMenuButtons, (float)screenWidth, (float)screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	
	deathScreen = new Menu();
	if (!deathScreen)
	{
		return false;
	}

	result = deathScreen->Initialize(direct3D->GetDevice(), hwnd, hinstance, L"menuBgrd_dead", nullptr, (float)screenWidth, (float)screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	winScreen = new Menu();
	if (!winScreen)
	{
		return false;
	}

	result = winScreen->Initialize(direct3D->GetDevice(), hwnd, hinstance, L"menuBgrd_win", nullptr, (float)screenWidth, (float)screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	loadScreen = new Menu();
	if (!loadScreen)
	{
		return false;
	}

	result = loadScreen->Initialize(direct3D->GetDevice(), hwnd, hinstance, L"menuBgrd_load", nullptr, (float)screenWidth, (float)screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	return true;
}

void System::Shutdown()
{
	if (menu)
	{
		menu->Shutdown();
		delete menu;
		menu = 0;
	}

	if (pauseMenu)
	{
		pauseMenu->Shutdown();
		delete pauseMenu;
		pauseMenu = 0;
	}
	
	if (deathScreen)
	{
		deathScreen->Shutdown();
		delete deathScreen;
		deathScreen = 0;
	}

	if (winScreen)
	{
		winScreen->Shutdown();
		delete winScreen;
		winScreen = 0;
	}

	if (loadScreen)
	{
		loadScreen->Shutdown();
		delete loadScreen;
		loadScreen = 0;
	}

	if (gamePlay != nullptr)
	{
		gamePlay->Shutdown();
		delete gamePlay;
		gamePlay = nullptr;
	}

	if (direct3D)
	{
		direct3D->Shutdown();
		delete direct3D;
		direct3D = 0;
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
			frameCount++;
			if (GetTime() > 1.0f)
			{
				fps = frameCount;
				frameCount = 0;
				StartTimer();
			}

			frameTime = GetFrameTime();

			// Otherwise do the frame processing.
			result = Frame(frameTime);
			if (!result)
			{
				done = true;
			}
		}

		// Check if the user pressed escape and wants to quit.
		/*if (player->IsEscapePressed() == true)
		{
			done = true;
		}*/
	}

	return;

}

bool System::Frame(double _time)
{
	int state;

#pragma region Update

	switch (gameState)
	{
	case GameState::gGamePlay:

		state = gamePlay->Update(_time);
		//timer->Update((double)fps);

		switch (state)
		{
		case 1:
			gameState = GameState::gPause;
			break;
		case 2:
			gameState = GameState::gDeath;
			prevState = GameState::gGamePlay;
		case 3: //VICTORY
			if (currentLevel < levels.size() - 1)
			{
				gameState = GameState::gLoading;
				currentLevel++;
			}
			else
			{
				gameState = GameState::gWin;
				currentLevel = 0;
			}
		}
		prevState = GameState::gGamePlay;
	break;

	case GameState::gMenu:
		state = menu->Update();

		switch (state)
		{
		case 1:
			gameState = GameState::gLoading;
			prevState = GameState::gMenu;
			break;
		case 2:
			return false;
		}

		break;

	case GameState::gPause:
		state = pauseMenu->Update();

		switch (state)
		{
		case 1:
			gameState = GameState::gGamePlay;
			break;
		case 2:
			gameState = GameState::gMenu;
			currentLevel = 0;
			gamePlay->Shutdown();
			delete gamePlay;
			gamePlay = nullptr;
		}

		break;

	case GameState::gDeath:
		if (prevState == GameState::gGamePlay)
		{
			gamePlay->Shutdown();
			delete gamePlay;
			gamePlay = new GamePlay();
			gamePlay->Initialize(direct3D->GetDevice(), hwnd, hinstance, levels.at(currentLevel).scene, levels.at(currentLevel).collision, levels.at(currentLevel).kristall, levels.at(currentLevel).winPlane, currentLevel);
			gameState = GameState::gGamePlay;
		}
		prevState = GameState::gDeath;
		break;

	case GameState::gLoading:
		if (prevState == GameState::gGamePlay)
		{
			gamePlay->Shutdown();
			delete gamePlay;
			gamePlay = new GamePlay();
			gamePlay->Initialize(direct3D->GetDevice(), hwnd, hinstance, levels.at(currentLevel).scene, levels.at(currentLevel).collision, levels.at(currentLevel).kristall, levels.at(currentLevel).winPlane, currentLevel);
			gameState = GameState::gGamePlay;
		}
		else if(prevState == GameState::gMenu)
		{
			gamePlay = new GamePlay();
			gamePlay->Initialize(direct3D->GetDevice(), hwnd, hinstance, levels.at(currentLevel).scene, levels.at(currentLevel).collision, levels.at(currentLevel).kristall, levels.at(currentLevel).winPlane, currentLevel);
			gameState = GameState::gGamePlay;
		}
		prevState = GameState::gLoading;
		break;

	default:
			break;
	}
#pragma endregion

#pragma region Draw

	direct3D->BeginScene(1.0f, 0.0f, 0.0f, 1.0f);

	
	switch (gameState)
	{
	case GameState::gGamePlay:
		gamePlay->RenderHUD(direct3D, timer);
		direct3D->DeferredFirstPass();
		gamePlay->Render(direct3D, timer);
		direct3D->DeferredRender();
		break;

	case GameState::gMenu:
		menu->Render(direct3D);
		break;

	case GameState::gPause:
		pauseMenu->Render(direct3D);
		break;	
	
	case GameState::gDeath:
		deathScreen->Render(direct3D);
		break;
	case GameState::gWin:
		winScreen->Render(direct3D);
		break;

	case GameState::gLoading:
		loadScreen->Render(direct3D);
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

	applicationName = L"The Legend Of Blonic: Hookarina of Time";


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


void System::StartTimer()
{
	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);

	countsPerSecond = double(frequencyCount.QuadPart);

	QueryPerformanceCounter(&frequencyCount);
	counterStart = frequencyCount.QuadPart;
}

double System::GetTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - counterStart) / countsPerSecond;
}

double System::GetFrameTime()
{
	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart - frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if (tickCount < 0.0f)
	{
		tickCount = 0;
	}

	return float(tickCount) / countsPerSecond;
}