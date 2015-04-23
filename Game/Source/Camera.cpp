#include "Camera.h"

Camera::Camera()
{
	m_defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f );
	m_defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f );
	m_cameraForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	m_cameraRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f );

	m_camPos = XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
	m_camLook = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	m_camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(m_camPos, m_camLook, m_camUp));
}


Camera::~Camera()
{
}


void Camera::Move(XMFLOAT2* _movement, XMFLOAT2 _rotation)
{
	// Rotation matrix from mouse input
	XMMATRIX cameraRotationMatrix = XMMatrixRotationRollPitchYaw(_rotation.y, _rotation.x, 0.0f);

	// Align vectors and look at position according to mouse input
	m_camLook = XMVector3TransformCoord(m_defaultForward, cameraRotationMatrix);
	m_camLook =XMVector3Normalize(m_camLook);

	m_cameraRight = XMVector3TransformCoord(m_defaultRight, cameraRotationMatrix);
	m_cameraForward = XMVector3TransformCoord(m_defaultForward, cameraRotationMatrix);

	m_camUp = XMVector3Cross(m_cameraForward, m_cameraRight);

	// New position from keyboard inputs
	m_camPos += _movement->x * m_cameraRight;
	m_camPos += _movement->y * m_cameraForward;

	// Reset input movement variable
	_movement->x = 0.0f;
	_movement->y = 0.0f;

	// Position look at accordingly to keyboard input
	m_camLook += m_camPos;

	// Create new view matrix
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(m_camPos, m_camLook, m_camUp));
}

void Camera::SetPosition(float _x, float _y, float _z)
{
	m_camPos = XMVectorSet(_x, _y, _z, 0.0f);
}

XMFLOAT3 Camera::GetPosition()
{
	XMFLOAT3 _camPos;
	XMStoreFloat3(&_camPos, m_camPos);

	return _camPos;
}

void Camera::Render()
{
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookAtLH(m_camPos, m_camLook, m_camUp));
}

XMMATRIX Camera::GetViewMatrix()
{
	return XMLoadFloat4x4(&m_viewMatrix);
}

XMFLOAT3 Camera::getCamLook()
{
	XMFLOAT3 _camLook;
	XMStoreFloat3(&_camLook, m_camLook);

	return _camLook;
}