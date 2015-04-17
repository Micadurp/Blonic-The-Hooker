#include "Camera.h"

Camera::Camera()
{
	XMStoreFloat4x4(&viewMatrix, XMMatrixIdentity());

	camPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	camLook = XMFLOAT3(0.0f, 0.0f, 1.0f);
	camUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
}


Camera::~Camera()
{
}


void Camera::SetPosition(float x, float y, float z)
{
	camPos = XMFLOAT3(x, y, z);
}

XMFLOAT3 Camera::GetPosition()
{
	return camPos;
}
void Camera::Render()
{
	XMStoreFloat4x4(&viewMatrix, XMMatrixLookAtLH(XMLoadFloat3(&camPos), XMLoadFloat3(&camLook), XMLoadFloat3(&camUp)));
}

XMMATRIX Camera::GetViewMatrix()
{
	return XMLoadFloat4x4(&viewMatrix);
}

XMFLOAT3 Camera::getCamLook()
{
	return camLook;
}