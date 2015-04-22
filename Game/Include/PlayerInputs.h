#ifndef PLAYERINPUTS_H
#define PLAYERINPUTS_H

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#include <dinput.h>
#include <DirectXMath.h>

using namespace DirectX;

class PlayerInputs
{
	private:
		XMFLOAT2 in_yawPitch;
		XMFLOAT2 in_movement;

		bool in_isEscapePressed;

		bool in_escapeStillPressed;
		bool in_returnStillPressed;
		bool in_spaceStillPressed;

		IDirectInputDevice8* in_keyboard;
		IDirectInputDevice8* in_mouse;

		DIMOUSESTATE in_mouseLastState;
		LPDIRECTINPUT8 in_directInput;

		void UpdateKeyboardStates(BYTE* keyboard);

	public:
		PlayerInputs();
		~PlayerInputs();

		bool Initialize(HWND &wndHandle, HINSTANCE &hInstance);
		void Update(double time);

		XMFLOAT2 GetYawPitch() const;
		XMFLOAT2 GetMovement() const;
		bool IsEscapePressed() const;

		void ReleaseCOM();
};

#endif