#ifndef MENU_H
#define MENU_H

#include "Direct3D.h"
#include "Button.h"
#include "Camera.h"
#include "PlayerInputs.h"

class Menu
{
	private:
		int buttonCount;

		KeyboardStateStruct lastKeyboardState;

		Model* menu_background;

		int currentBtn;

		Model* menuSelector;
		float selectorPosition;

		std::vector<Button*> menuButtons;

		Camera* camera;
		PlayerInputs* input;

		XMFLOAT4X4 projectionMatrix;

	public:
		Menu();
		~Menu();

		bool Initialize(ID3D11Device* _device,int nrOf, HWND &_wndHandle, HINSTANCE &_hInstance, wstring _background, wstring* _buttons = nullptr, float _width = 0.0f, float _height = 0.0f, float _nearZ = 0.0f, float _farZ = 0.0f);
		void Shutdown();

		int Update();

		void Render(Direct3D* _direct3D);
};
#endif