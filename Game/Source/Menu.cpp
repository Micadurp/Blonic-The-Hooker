#include"Menu.h"

Menu::Menu()
{
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

	result = camera->Initialize();
	if (!result)
	{
		return false;
	}

	result = input->Initialize(wndHandle, hInstance);
	if (!result)
	{
		return false;
	}

	for (int i = 0; i < 2; i++)
	{
		menuButtons[i]->Initialize(_device, i, L"menuBtn_obj", &collidableGeometryPositions, &collidableGeometryIndices, XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(0.0f, 0.0f, 5.0f));
	}

	XMStoreFloat4x4(&projectionMatrix, XMMatrixOrthographicLH(_width, _height, _nearZ, _farZ));

	return true;
}

int Menu::Update()
{
	return -1;
}

void Menu::Render()
{

}