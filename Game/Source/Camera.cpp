#include "Camera.h"

Camera::Camera()
{
	m_camPos = { 0.0f, 0.0f, -1.0f, 0.0f };
	m_camLook = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_camUp = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(XMLoadFloat4(&m_camPos), XMLoadFloat4(&m_camLook), XMLoadFloat4(&m_camUp)));
}


Camera::~Camera()
{
}

bool Camera::Initialize()
{
	return true;
}

void Camera::UpdateViewMatrix(const XMFLOAT4& _camPos, const XMFLOAT4& _camLook, const XMFLOAT4& _camUp)
{
	// Create new view matrix
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(XMLoadFloat4(&_camPos), XMLoadFloat4(&_camLook), XMLoadFloat4(&_camUp)));
}

void Camera::Update(const double time)
{
}

void Camera::SetPosition(const float _x, const float _y, const float _z)
{
	m_camPos = { _x, _y, _z, 0.0f };
}


const XMFLOAT4& Camera::GetPosition() const
{
	return m_camPos;
}

const XMFLOAT4X4& Camera::GetViewMatrix() const
{
	return m_viewMatrix;
}

const XMFLOAT4& Camera::GetCamLook() const
{
	return m_camLook;
}