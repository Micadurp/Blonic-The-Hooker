#include"Menu.h"

Menu::Menu()
{
	menu_background = new Model();
	menuButtons = new Button*[buttonCount];

	menuSelector = new Model();
	selectorPosition = -0.2f;

	camera = new Camera();
	input = new PlayerInputs();

	for (int i = 0; i < buttonCount; i++)
	{
		menuButtons[i] = new Button();
	}

	XMStoreFloat4x4(&projectionMatrix, XMMatrixIdentity());
}

Menu::~Menu()
{
	for (int i = 0; i < buttonCount; i++)
	{
		delete menuButtons[i];
		menuButtons[i] = nullptr;
	}

	delete[] menuButtons;
	delete camera;
	delete input;
}

bool Menu::Initialize(ID3D11Device* _device, HWND &_wndHandle, HINSTANCE &_hInstance, wstring _background, wstring* _buttons, float _width, float _height, float _nearZ, float _farZ)
{
	bool result;

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

	// Initialize menu selector
	result = menuSelector->Initialize(L"menuSelector", _device);
	if (!result)
	{
		return false;
	}
	menuSelector->SetObjMatrix(XMMatrixScaling(0.8f, 0.8f, 0.8f) * XMMatrixTranslation(-0.5f, selectorPosition, 1.0f));

	// Initialize menu buttons
	for (int i = 0; i < buttonCount; i++)
	{
		result = menuButtons[i]->Initialize(_device, _buttons[i], XMMatrixScaling(0.8f, 0.8f, 0.8f) * XMMatrixTranslation(0.0f, (i * (-0.2f)) - 0.2f, 1.0f));
		if (!result)
		{
			return false;
		}
	}

	// Initialize background image
	result = menu_background->Initialize(_background, _device);
	if (!result)
	{
		return false;
	}
	menu_background->SetObjMatrix(XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(0.0f, 0.0f, 1.4f));

	return true;
}

int Menu::Update()
{
	lastKeyboardState = input->GetKeyboardState();

	input->Update();

	if ((input->GetKeyboardState().key_w_pressed && !lastKeyboardState.key_w_pressed) || (input->GetKeyboardState().key_s_pressed && !lastKeyboardState.key_s_pressed))
	{
		if (selectorPosition < -0.2f)
		{
			selectorPosition = -0.2f;
		}
		else
		{
			selectorPosition = -0.4f;
		}

		menuSelector->SetObjMatrix(XMMatrixScaling(0.8f, 0.8f, 0.8f) * XMMatrixTranslation(-0.5f, selectorPosition, 1.0f));
	}

	if (input->GetKeyboardState().key_enter_pressed && !lastKeyboardState.key_enter_pressed)
	{
		// New game
		if (selectorPosition > -0.4f)
		{
			return 1;
		}
		// Quit game
		else
		{
			return 2;
		}
	}

	return -1;
}

void Menu::Render(Direct3D* _direct3D)
{
	_direct3D->SetShader();

	// Render menu background
	_direct3D->SetVertexCBuffer(menu_background->GetObjMatrix(), XMLoadFloat4x4(&camera->GetViewMatrix()));
	menu_background->Render(_direct3D->GetDeviceContext());

	// Render menu buttons
	for (int i = 0; i < buttonCount; i++)
	{
		_direct3D->SetVertexCBuffer(menuButtons[i]->GetObjMatrix(), XMLoadFloat4x4(&camera->GetViewMatrix()));
		menuButtons[i]->Render(_direct3D->GetDeviceContext());
	}

	// Render menu selector
	_direct3D->SetCrosshairShaders();

	_direct3D->SetVertexCBuffer(menuSelector->GetObjMatrix(), XMLoadFloat4x4(&camera->GetViewMatrix()));
	menuSelector->Render(_direct3D->GetDeviceContext());
}