#ifndef MENU_H
#define MENU_H

#include "Button.h"
#include "Camera.h"
#include "PlayerInputs.h"

class Menu
{
	private:
		Button** menuButtons;
		Camera* camera;
		PlayerInputs* input;

		XMFLOAT4X4 projectionMatrix;

		vector<XMFLOAT3> collidableGeometryPositions;
		vector<DWORD> collidableGeometryIndices;

	public:
		Menu();
		~Menu();

		bool Initialize(ID3D11Device* _device, HWND &wndHandle, HINSTANCE &hInstance, float _width, float _height, float _nearZ, float _farZ);

		int Update();

		void Render();
};
#endif