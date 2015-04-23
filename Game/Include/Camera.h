#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
private:
	XMFLOAT4X4 m_viewMatrix;

	XMVECTOR m_defaultForward;
	XMVECTOR m_defaultRight;
	XMVECTOR m_cameraForward;
	XMVECTOR m_cameraRight;

	XMVECTOR m_camPos;
	XMVECTOR m_camLook;
	XMVECTOR m_camUp;

public:
	Camera();
	~Camera();

	void Move(XMFLOAT2* _movement, XMFLOAT2 _rotation);

	void SetPosition(float _x, float _y, float _z);
	XMFLOAT3 GetPosition();

	void Render();
	XMMATRIX GetViewMatrix();

	XMFLOAT3 getCamLook();
};

#endif