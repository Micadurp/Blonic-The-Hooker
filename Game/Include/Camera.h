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

	void UpdateViewMatrix(const XMFLOAT4& camPos, const XMFLOAT4& camLook, const XMFLOAT4& camUp);

public:
	Camera();
	virtual ~Camera();

	void SetPosition(const float _x, const float _y, const float _z);
	const XMFLOAT4& GetPosition() const;

	const XMFLOAT4X4& GetViewMatrix() const;
	const XMFLOAT4& GetCamLook() const;
};

#endif