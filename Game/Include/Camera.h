#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
private:
	XMFLOAT4X4 viewMatrix;

	XMFLOAT3 camPos;
	XMFLOAT3 camLook;
	XMFLOAT3 camUp;

public:
	Camera();
	~Camera();

	void SetPosition(float x, float y, float z);
	XMFLOAT3 GetPosition();

	void Render();
	XMMATRIX GetViewMatrix();


	XMFLOAT3 getCamLook();
};

#endif