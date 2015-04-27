#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

class Camera
{
protected:
	XMFLOAT4X4 m_viewMatrix;

	XMFLOAT4 m_camPos;
	XMFLOAT4 m_camLook;
	XMFLOAT4 m_camUp;

	void UpdateViewMatrix(XMFLOAT4 camPos, XMFLOAT4 camLook, XMFLOAT4 camUp);

public:
	Camera();
	virtual ~Camera();

	void SetPosition(float _x, float _y, float _z);
	XMFLOAT4 GetPosition();

	virtual void Update(double time);

	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4 GetCamLook();
};

#endif