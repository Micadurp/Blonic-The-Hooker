#include "Camera.h"

Camera::Camera()
{
	m_camPos = { 0.0f, 0.0f, -5.0f, 0.0f };
	m_camLook = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_camUp = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(XMLoadFloat4(&m_camPos), XMLoadFloat4(&m_camLook), XMLoadFloat4(&m_camUp)));
}


Camera::~Camera()
{
}

void Camera::UpdateViewMatrix(XMFLOAT4 _camPos, XMFLOAT4 _camLook, XMFLOAT4 _camUp)
{
	// Create new view matrix
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(XMLoadFloat4(&_camPos), XMLoadFloat4(&_camLook), XMLoadFloat4(&_camUp)));
}

void Camera::SetPosition(float _x, float _y, float _z)
{
	m_camPos = { _x, _y, _z, 0.0f };
}

void Camera::Update(double time)
{

}

XMFLOAT4 Camera::GetPosition()
{
	return m_camPos;
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return m_viewMatrix;
}

XMFLOAT4 Camera::GetCamLook()
{
	return m_camLook;
}