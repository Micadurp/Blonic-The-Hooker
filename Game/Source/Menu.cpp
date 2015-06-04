#include"Menu.h"

Menu::Menu()
{
	menu_background = new Model();

	menuSelector = nullptr;
	selectorPosition = -0.2f;

	camera = new Camera();
	input = new PlayerInputs();


	XMStoreFloat4x4(&projectionMatrix, XMMatrixIdentity());
}

Menu::~Menu()
{
}

bool Menu::Initialize(ID3D11Device* _device, int nrOf, HWND &_wndHandle, HINSTANCE &_hInstance, wstring _background, wstring* _buttons, float _width, float _height, float _nearZ, float _farZ)
{
	bool result;

	currentBtn = 0;
	buttonCount = nrOf;

	if (_width > 0.0f && _height > 0.0f && _nearZ > 0.0f && _farZ > 0.0f)
	{
		XMStoreFloat4x4(&projectionMatrix, XMMatrixOrthographicLH(_width, _height, _nearZ, _farZ));
	}
	else
	{
		XMStoreFloat4x4(&projectionMatrix, XMMatrixIdentity());
	}

	
	result = input->Initialize(_wndHandle, _hInstance);
	if (!result)
	{
		return false;
	}

	if (_buttons)
	{
		menuSelector = new Model();

		// Initialize menu selector
		result = menuSelector->Initialize(L"menuSelector", _device);
		if (!result)
		{
			return false;
		}
		menuSelector->SetObjMatrix(XMMatrixScaling(0.8f, 0.8f, 1.0f) * XMMatrixTranslation(-0.5f, selectorPosition, 0.0f));


		for (int i = 0; i < buttonCount; i++)
		{
			menuButtons.push_back(new Button());
		}

		// Initialize menu buttons
		for (int i = 0; i < buttonCount; i++)
		{
			result = menuButtons.at(i)->Initialize(_device, _buttons[i], XMMatrixScaling(0.8f, 0.8f, 1.0f) * XMMatrixTranslation(0.0f, (i * (-0.2f)) - 0.2f, 0.0f));
			if (!result)
			{
				return false;
			}
		}
	}

	// Initialize background image
	result = menu_background->Initialize(_background, _device);
	if (!result)
	{
		return false;
	}
	menu_background->SetObjMatrix(XMMatrixScaling(0.7f, 1.0f, 1.0f) * XMMatrixTranslation(0.0f, 0.0f, 0.1f));

	return true;
}

void Menu::Shutdown()
{
	if (menu_background)
	{
		menu_background->Shutdown();
		delete menu_background;
		menu_background = 0;
	}	
	
	if (menuSelector)
	{
		menuSelector->Shutdown();
		delete menuSelector;
		menuSelector = 0;
	}	
	
	while (!menuButtons.empty())
	{
		menuButtons.back()->Shutdown();
		delete menuButtons.back();
		menuButtons.pop_back();
	}

	if (camera)
	{
		delete camera;
		camera = 0;
	}

	if (input)
	{
		delete input;
		input = 0;
	}
}

int Menu::Update()
{
	lastKeyboardState = input->GetKeyboardState();

	input->Update();

	if ((input->GetKeyboardState().key_w_pressed && !lastKeyboardState.key_w_pressed))
	{
		currentBtn--;
		if (currentBtn < 0)
			currentBtn = buttonCount - 1;
	}

	if ((input->GetKeyboardState().key_s_pressed && !lastKeyboardState.key_s_pressed))
	{
		currentBtn++;
		if (currentBtn >= buttonCount)
			currentBtn = 0;
		
	}
	if (menuSelector)
		menuSelector->SetObjMatrix(XMMatrixScaling(0.8f, 0.8f, 1.0f) * XMMatrixTranslation(-0.5f, -0.2f +(currentBtn * -0.2f), 0.0f));


	if (input->GetKeyboardState().key_enter_pressed && !lastKeyboardState.key_enter_pressed)
	{
		return currentBtn;
	}

	return -1;
}

void Menu::Render(Direct3D* _direct3D)
{
	// Render menu selector
	_direct3D->SetCrosshairShaders();

	if (menuSelector)
	{
		_direct3D->SetVertexCBuffer(menuSelector->GetObjMatrix());
		menuSelector->Render(_direct3D->GetDeviceContext());
	}

	// Render menu background
	_direct3D->SetVertexCBuffer(menu_background->GetObjMatrix());
	menu_background->Render(_direct3D->GetDeviceContext());

	// Render menu buttons
	for (size_t i = 0; i < menuButtons.size(); i++)
	{
		_direct3D->SetVertexCBuffer(menuButtons.at(i)->GetObjMatrix());
		menuButtons.at(i)->Render(_direct3D->GetDeviceContext());
	}	
}