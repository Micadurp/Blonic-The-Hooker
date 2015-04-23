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
		XMFLOAT2 m_yawPitch;
		XMFLOAT2 m_movement;

		bool m_escapeStillPressed;
		bool m_returnStillPressed;
		bool m_spaceStillPressed;

		IDirectInputDevice8* m_keyboard;
		IDirectInputDevice8* m_mouse;

		DIMOUSESTATE m_mouseLastState;
		LPDIRECTINPUT8 m_directInput;

		void UpdateKeyboardStates(BYTE* keyboard);

	public:
		PlayerInputs();
		~PlayerInputs();

		bool Initialize(HWND &wndHandle, HINSTANCE &hInstance);
		void Update(double time);

		XMFLOAT2 GetYawPitch() const;
		XMFLOAT2* GetMovement();

		void ReleaseCOM();
};

#endif