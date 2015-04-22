#include "PlayerInputs.h"

PlayerInputs::PlayerInputs()
{
	in_yawPitch = { 0.0f, 0.0f };		// x = yaw rotation, y = pitch
	in_movement = { 0.0f, 0.0f };		// x = left/right, y = forward/backward

	in_keyboard = nullptr;
	in_mouse = nullptr;

	in_escapeStillPressed = false;
}

PlayerInputs::~PlayerInputs()
{
}

bool PlayerInputs::Initialize(HWND &wndHandle, HINSTANCE &hInstance)
{
	HRESULT hr;

	DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&in_directInput, NULL);

	// Initialize keyboard device
	hr = in_directInput->CreateDevice(GUID_SysKeyboard, &in_keyboard, NULL);
	if (FAILED(hr))
	{
		MessageBox(wndHandle, L"Input keyboard device", L"Failed to create", MB_OK);
		return false;
	}

	// Initialize mouse device
	hr = in_directInput->CreateDevice(GUID_SysMouse, &in_mouse, NULL);
	if (FAILED(hr))
	{
		MessageBox(wndHandle, L"Input mouse device", L"Failed to create", MB_OK);
		return false;
	}

	// Set keyboard data format, expected input
	hr = in_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
	{
		MessageBox(wndHandle, L"Keyboard data format", L"Failed to set", MB_OK);
		return false;
	}

	hr = in_keyboard->SetCooperativeLevel(wndHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
	{
		return false;
	}

	// Set mouse data format, expected input
	hr = in_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr))
	{
		MessageBox(wndHandle, L"Mouse data format", L"Failed to set", MB_OK);
		return false;
	}

	hr = in_mouse->SetCooperativeLevel(wndHandle, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void PlayerInputs::Update(double time)
{
	DIMOUSESTATE mouseCurrentState;
	BYTE keyboardState[256];

	in_keyboard->Acquire();
	in_mouse->Acquire();

	// Store mouse current state
	in_mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrentState);

	// Store keyboard current state
	in_keyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	float speed = 40.0f * time;

	// ------------------------------
	//
	// PLAYER INPUTS
	//
	// ------------------------------

	// Keyboard inputs --------------
	if (keyboardState[DIK_A] & 0x80)
	{
		in_movement.x -= speed;
	}
	if (keyboardState[DIK_D] & 0x80)
	{
		in_movement.x += speed;
	}
	if (keyboardState[DIK_W] & 0x80)
	{
		in_movement.y += speed;
	}
	if (keyboardState[DIK_S] & 0x80)
	{
		in_movement.y -= speed;
	}
	if (keyboardState[DIK_SPACE] & 0x80)
	{
		// Do stuff
	}
	if (keyboardState[DIK_RETURN] & 0x80)
	{
		// Do stuff
	}
	if (keyboardState[DIK_ESCAPE] & 0x80)
	{
		// Do stuff
	}
	// ------------------------------

	// Mouse inputs -----------------
	// Mouse movement controls rotation
	if (mouseCurrentState.lX != in_mouseLastState.lX || mouseCurrentState.lY != in_mouseLastState.lY)
	{
		in_yawPitch.x += in_mouseLastState.lX * 0.001f;
		in_yawPitch.y += in_mouseLastState.lY * 0.001f;

		in_mouseLastState = mouseCurrentState;
	}

	// Mouse left click event
	if (mouseCurrentState.rgbButtons[0])
	{
		// Do stuff
	}

	// Mouse right click event
	if (mouseCurrentState.rgbButtons[1])
	{
		// Do stuff
	}
	// ------------------------------

	UpdateKeyboardStates(keyboardState);
}

void PlayerInputs::UpdateKeyboardStates(BYTE* keyboardState)
{
	in_escapeStillPressed = keyboardState[DIK_ESCAPE] & 0x80;
	in_returnStillPressed = keyboardState[DIK_RETURN] & 0x80;
	in_spaceStillPressed = keyboardState[DIK_SPACE] & 0x80;
}

XMFLOAT2 PlayerInputs::GetYawPitch() const
{
	return in_yawPitch;
}

XMFLOAT2 PlayerInputs::GetMovement() const
{
	return in_movement;
}

void PlayerInputs::ReleaseCOM()
{
	in_keyboard->Unacquire();
	in_mouse->Unacquire();
	in_directInput->Release();
}