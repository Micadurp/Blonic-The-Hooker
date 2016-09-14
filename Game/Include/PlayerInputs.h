#ifndef PLAYERINPUTS_H
#define PLAYERINPUTS_H

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <DirectXMath.h>
#include "Structs\InputStructure.h"

using namespace DirectX;

class PlayerInputs
{
	private:
		KeyboardStateStruct m_keyboardStateObject;
		MouseStateStruct m_mouseStateObject;

		bool m_isEscapePressed;
		bool m_isMousePressed;

		bool m_escapeStillPressed;
		bool m_returnStillPressed;
		bool m_spaceStillPressed;

		IDirectInputDevice8* m_keyboard;
		IDirectInputDevice8* m_mouse;

		DIMOUSESTATE m_mouseLastState;
		LPDIRECTINPUT8 m_directInput;

		void UpdateInputStates(BYTE* keyboard, DIMOUSESTATE mouseState);

	public:
		PlayerInputs();
		~PlayerInputs();

		bool Initialize(HWND &wndHandle, HINSTANCE &hInstance);
		void Update();

		KeyboardStateStruct GetKeyboardState() const;
		MouseStateStruct GetMouseState() const;

		bool IsEscapePressed() const;
		bool LeftMouseClick() const;

		void ReleaseCOM();
};

#endif