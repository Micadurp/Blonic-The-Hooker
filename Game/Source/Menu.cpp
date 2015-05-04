#include"Menu.h"

Menu::Menu()
{
	menu_background = new Model();
	menuButtons = new Button*[2];

	camera = new Camera();
	input = new PlayerInputs();

	for (int i = 0; i < 2; i++)
	{
		menuButtons[i] = new Button();
	}

	XMStoreFloat4x4(&projectionMatrix, XMMatrixIdentity());
}

Menu::~Menu()
{
	for (int i = 0; i < 2; i++)
	{
		delete menuButtons[i];
		menuButtons[i] = nullptr;
	}

	delete[] menuButtons;
	delete camera;
	delete input;
}

bool Menu::Initialize(ID3D11Device* _device, HWND &wndHandle, HINSTANCE &hInstance, float _width, float _height, float _nearZ, float _farZ)
{
	bool result;

	result = input->Initialize(wndHandle, hInstance);
	if (!result)
	{
		return false;
	}

	// Initialize background image
	menu_background->Initialize(L"menuBgrd_obj_LH", _device);
	menu_background->SetObjMatrix(XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(0.0f, 0.0f, 1.4f));

	XMStoreFloat4x4(&projectionMatrix, XMMatrixOrthographicLH(_width, _height, _nearZ, _farZ));

	return true;
}

int Menu::Update()
{
	input->Update();

	if (input->GetKeyboardState().key_escape_pressed)
	{
		return 1;
	}

	return -1;
}

void Menu::Render(Direct3D* _direct3D)
{
	_direct3D->SetShader();

	_direct3D->SetVertexCBuffer(menu_background->GetObjMatrix(), XMLoadFloat4x4(&camera->GetViewMatrix()));

	menu_background->Render(_direct3D->GetDeviceContext());
}