#include "Camera.h"

Camera::Camera()
{
	XMStoreFloat4x4(&viewMatrix, XMMatrixIdentity());

	camPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	camLook = XMFLOAT3(0.0f, 0.0f, 1.0f);
	camUp = XMFLOAT3(0.0f, 1.0f, 0.0f);

	gravity = XMFLOAT3(0.0f, -0.2f, 0.0f);
}


Camera::~Camera()
{
}


void Camera::SetPosition(float _x, float _y, float _z)
{
	camPos = XMFLOAT3(_x, _y, _z);
}

XMFLOAT3 Camera::GetPosition()
{
	return camPos;
}
void Camera::Update()
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

XMVECTOR Camera::Collision( std::vector<XMFLOAT3>& _vertPos, std::vector<DWORD>& _indices)
{
	CollisionPacket collisionPack;
	collisionPack.ellipsoidSpace = XMVectorSet(1.0f, 3.0f, 1.0f, 0.0f);
	collisionPack.w_Position = XMLoadFloat3(&camPos);
	collisionPack.w_Velocity = XMLoadFloat3(&velocity);

	// Transform velocity vector to the ellipsoid space (e_ denotes ellipsoid space)
	collisionPack.e_Velocity = collisionPack.w_Velocity / collisionPack.ellipsoidSpace;

	// Transform position vector to the ellipsoid space
	collisionPack.e_Position = collisionPack.w_Position / collisionPack.ellipsoidSpace;

	// Now we check for a collision with our world
	collisionPack.collisionRecursionDepth = 0;
	XMVECTOR finalPosition = CollideWithWorld(collisionPack, _vertPos, _indices);

	//// Add gravity pull
	collisionPack.e_Velocity = XMLoadFloat3(&gravity) / collisionPack.ellipsoidSpace;
	collisionPack.e_Position = finalPosition;
	collisionPack.collisionRecursionDepth = 0;
	finalPosition = CollideWithWorld(collisionPack, _vertPos, _indices);

	// Convert our final position from ellipsoid space to world space
	finalPosition = finalPosition * collisionPack.ellipsoidSpace;

	return finalPosition;
}


XMVECTOR Camera::CollideWithWorld(CollisionPacket& _cP, std::vector<XMFLOAT3>& _vertPos, std::vector<DWORD>& _indices)
{
	float veryCloseDistance = 0.005f;

	if (_cP.collisionRecursionDepth > 5)
		return _cP.e_Position;

	// Normalize velocity vector
	_cP.e_normalizedVelocity = XMVector3Normalize(_cP.e_Velocity);

	_cP.foundCollision = false;
	_cP.nearestDistance = 0.0f;

	// Loop through each triangle in mesh and check for a collision
	for (int triCounter = 0; triCounter < _indices.size() / 3; triCounter++)
	{
		XMVECTOR p0, p1, p2, tempVec;
		p0 = XMLoadFloat3(&_vertPos[_indices[3 * triCounter]]);
		p1 = XMLoadFloat3(&_vertPos[_indices[3 * triCounter + 1]]);
		p2 = XMLoadFloat3(&_vertPos[_indices[3 * triCounter + 2]]);

		// Put triangle into ellipsoid space
		p0 = p0 / _cP.ellipsoidSpace;
		p1 = p1 / _cP.ellipsoidSpace;
		p2 = p2 / _cP.ellipsoidSpace;

		// Calculate the normal for this triangle
		XMVECTOR triNormal;
		triNormal = XMVector3Normalize(XMVector3Cross((p1 - p0), (p2 - p0)));

		SphereCollidingWithTriangle(_cP, p0, p1, p2, triNormal);
	}

	// If there was no collision, return the position + velocity
	if (_cP.foundCollision == false) {
		return _cP.e_Position + _cP.e_Velocity;
	}

	XMVECTOR destinationPoint = _cP.e_Position + _cP.e_Velocity;

	XMVECTOR newPosition = _cP.e_Position;	// Just initialize newPosition

	if (_cP.nearestDistance >= veryCloseDistance)
	{
		// Move the new position down velocity vector to ALMOST touch the collision point
		XMVECTOR V = _cP.e_Velocity;
		V = XMVector3Normalize(V);
		V = V * (_cP.nearestDistance - veryCloseDistance);
		newPosition = _cP.e_Position + V;

		// Adjust polygon intersection point
		V = XMVector3Normalize(V);
		_cP.intersectionPoint -= veryCloseDistance * V;
	}

	// This is the sliding plane
	XMVECTOR slidePlaneOrigin = _cP.intersectionPoint;
	XMVECTOR slidePlaneNormal = newPosition - _cP.intersectionPoint;
	slidePlaneNormal = XMVector3Normalize(slidePlaneNormal);

	// First the point in the plane
	float x = XMVectorGetX(slidePlaneOrigin);
	float y = XMVectorGetY(slidePlaneOrigin);
	float z = XMVectorGetZ(slidePlaneOrigin);

	// Next the planes normal
	float A = XMVectorGetX(slidePlaneNormal);
	float B = XMVectorGetY(slidePlaneNormal);
	float C = XMVectorGetZ(slidePlaneNormal);
	float D = -((A*x) + (B*y) + (C*z));

	float planeConstant = D;

	// Get the distance between sliding plane and destination point
	float signedDistFromDestPointToSlidingPlane = XMVectorGetX(XMVector3Dot(destinationPoint, slidePlaneNormal)) + planeConstant;

	XMVECTOR newDestinationPoint = destinationPoint - signedDistFromDestPointToSlidingPlane * slidePlaneNormal;

	XMVECTOR newVelocityVector = newDestinationPoint - _cP.intersectionPoint;

	if (XMVectorGetX(XMVector3Length(newVelocityVector)) < veryCloseDistance) {
		return newPosition;
	}

	_cP.collisionRecursionDepth++;
	_cP.e_Position = newPosition;
	_cP.e_Velocity = newVelocityVector;
	return CollideWithWorld(_cP, _vertPos, _indices);
}


bool Camera::checkPointInTriangle(const XMVECTOR& _point, const XMVECTOR& _triV1, const XMVECTOR& _triV2, const XMVECTOR& _triV3)
{
	XMVECTOR cp1 = XMVector3Cross((_triV3 - _triV2), (_point - _triV2));
	XMVECTOR cp2 = XMVector3Cross((_triV3 - _triV2), (_triV1 - _triV2));
	if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
	{
		cp1 = XMVector3Cross((_triV3 - _triV1), (_point - _triV1));
		cp2 = XMVector3Cross((_triV3 - _triV1), (_triV2 - _triV1));
		if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
		{
			cp1 = XMVector3Cross((_triV2 - _triV1), (_point - _triV1));
			cp2 = XMVector3Cross((_triV2 - _triV1), (_triV3 - _triV1));
			if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
			{
				return true;
			}
		}
	}
	return false;
}

// This function solves the quadratic eqation "At^2 + Bt + C = 0"
bool Camera::getLowestRoot(float _a, float _b, float _c, float _maxR, float* _root)
{
	// Check if a solution exists
	float determinant = _b*_b - 4.0f*_a*_c;
	// If determinant is negative it means no solutions.
	if (determinant < 0.0f) return false;
	// calculate the two roots
	float sqrtD = sqrt(determinant);
	float r1 = (-_b - sqrtD) / (2 * _a);
	float r2 = (-_b + sqrtD) / (2 * _a);
	// Sort so x1 <= x2
	if (r1 > r2) {
		float temp = r2;
		r2 = r1;
		r1 = temp;
	}
	// Get lowest root:
	if (r1 > 0 && r1 < _maxR) {
		*_root = r1;
		return true;
	}
	// It is possible that we want x2 - this can happen
	// if x1 < 0
	if (r2 > 0 && r2 < _maxR) {
		*_root = r2;
		return true;
	}

	// No (valid) solutions
	return false;
}