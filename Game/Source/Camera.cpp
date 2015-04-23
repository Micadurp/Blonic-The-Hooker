#include "Camera.h"

Camera::Camera()
{
	m_defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f );
	m_defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f );
	m_cameraForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	m_cameraRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f );

	gravity = XMFLOAT3(0.0f, -0.2f, 0.0f);

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

void Camera::Update()
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

XMVECTOR Camera::Collision( std::vector<XMFLOAT3>& _vertPos, std::vector<DWORD>& _indices)
{
	CollisionPacket collisionPack;
	collisionPack.ellipsoidSpace = XMVectorSet(1.0f, 3.0f, 1.0f, 0.0f);
	collisionPack.w_Position = m_camPos;
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


bool Camera::SphereCollidingWithTriangle(CollisionPacket& _cP, XMVECTOR &_p0, XMVECTOR &_p1, XMVECTOR &_p2, XMVECTOR &_triNormal)
{
	// Check to see if triangle is facing velocity vector
	float facing = XMVectorGetX(XMVector3Dot(_triNormal, _cP.e_normalizedVelocity));
	if (facing <= 0)
	{
		XMVECTOR velocity = _cP.e_Velocity;
		XMVECTOR position = _cP.e_Position;

		float t0, t1;

		bool sphereInPlane = false;

		// First the point in the plane
		float x = XMVectorGetX(_p0);
		float y = XMVectorGetY(_p0);
		float z = XMVectorGetZ(_p0);

		// Next the planes normal
		float A = XMVectorGetX(_triNormal);
		float B = XMVectorGetY(_triNormal);
		float C = XMVectorGetZ(_triNormal);

		float D = -((A*x) + (B*y) + (C*z));

		float planeConstant = D;

		float signedDistFromPositionToTriPlane = XMVectorGetX(XMVector3Dot(position, _triNormal)) + planeConstant;

		float planeNormalDotVelocity = XMVectorGetX(XMVector3Dot(_triNormal, velocity));

		/////////////////////////////////////Sphere Plane Collision Test////////////////////////////////////////////
		// Check to see if the velocity vector is parallel with the plane
		if (planeNormalDotVelocity == 0.0f)
		{
			if (fabs(signedDistFromPositionToTriPlane) >= 1.0f)
			{
				// sphere not in plane, and velocity is
				// parallel to plane, no collision possible
				return false;
			}
			else
			{
				sphereInPlane = true;
			}
		}
		else
		{
			// finding when both front and back of sphere touches plane
			t0 = (1.0f - signedDistFromPositionToTriPlane) / planeNormalDotVelocity;
			t1 = (-1.0f - signedDistFromPositionToTriPlane) / planeNormalDotVelocity;

			// t0 will become the front
			if (t0 > t1)
			{
				float temp = t0;
				t0 = t1;
				t1 = temp;
			}

			// no intersect this frame
			if (t0 > 1.0f || t1 < 0.0f)
			{
				return false;
			}

			// If t0 is smaller than 0 then we will make it 0
			// and if t1 is greater than 1 we will make it 1
			if (t0 < 0.0) t0 = 0.0;
			if (t1 > 1.0) t1 = 1.0;
		}

		////////////////////////////////Sphere-(Inside Triangle) Collision Test///////////////////////////////////////
		XMVECTOR collisionPoint;		// Point on plane where collision occured
		bool collidingWithTri = false;
		float t = 1.0;					// Time 

		// If the sphere is not IN the triangles plane, we continue the sphere to inside of triangle test
		if (!sphereInPlane)
		{
			XMVECTOR planeIntersectionPoint = (position + t0 * velocity - _triNormal);

			// Now we call the function that checks if a point on a triangle's plane is inside the triangle
			if (checkPointInTriangle(planeIntersectionPoint, _p0, _p1, _p2))
			{
				collidingWithTri = true;
				t = t0;
				collisionPoint = planeIntersectionPoint;
			}
		}

		/////////////////////////////////////Sphere-Vertex Collision Test//////////////////////////////////////////////
		if (collidingWithTri == false)
		{
			float a, b, c; // Equation Parameters

			float velocityLengthSquared = XMVectorGetX(XMVector3Length(velocity));
			velocityLengthSquared *= velocityLengthSquared;

			a = velocityLengthSquared;

			float newT;

			// P0 - Collision test with sphere and p0
			b = 2.0f * (XMVectorGetX(XMVector3Dot(velocity, position - _p0)));
			c = XMVectorGetX(XMVector3Length((_p0 - position)));
			c = (c*c) - 1.0f;
			if (getLowestRoot(a, b, c, t, &newT)) {	// Check if the equation can be solved
				t = newT;
				collidingWithTri = true;
				collisionPoint = _p0;
			}

			// P1 - Collision test with sphere and p1
			b = 2.0*(XMVectorGetX(XMVector3Dot(velocity, position - _p1)));
			c = XMVectorGetX(XMVector3Length((_p1 - position)));
			c = (c*c) - 1.0;
			if (getLowestRoot(a, b, c, t, &newT)) {
				t = newT;
				collidingWithTri = true;
				collisionPoint = _p1;
			}

			// P2 - Collision test with sphere and p2
			b = 2.0*(XMVectorGetX(XMVector3Dot(velocity, position - _p2)));
			c = XMVectorGetX(XMVector3Length((_p2 - position)));
			c = (c*c) - 1.0;
			if (getLowestRoot(a, b, c, t, &newT)) {
				t = newT;
				collidingWithTri = true;
				collisionPoint = _p2;
			}

			//////////////////////////////////////////////Sphere-Edge Collision Test//////////////////////////////////////////////

			// Edge (p0, p1):
			XMVECTOR edge = _p1 - _p0;
			XMVECTOR spherePositionToVertex = _p0 - position;
			float edgeLengthSquared = XMVectorGetX(XMVector3Length(edge));
			edgeLengthSquared *= edgeLengthSquared;
			float edgeDotVelocity = XMVectorGetX(XMVector3Dot(edge, velocity));
			float edgeDotSpherePositionToVertex = XMVectorGetX(XMVector3Dot(edge, spherePositionToVertex));
			float spherePositionToVertexLengthSquared = XMVectorGetX(XMVector3Length(spherePositionToVertex));
			spherePositionToVertexLengthSquared = spherePositionToVertexLengthSquared * spherePositionToVertexLengthSquared;

			// Equation parameters
			a = edgeLengthSquared * -velocityLengthSquared + (edgeDotVelocity * edgeDotVelocity);
			b = edgeLengthSquared * (2.0f * XMVectorGetX(XMVector3Dot(velocity, spherePositionToVertex))) - (2.0f * edgeDotVelocity * edgeDotSpherePositionToVertex);
			c = edgeLengthSquared * (1.0f - spherePositionToVertexLengthSquared) + (edgeDotSpherePositionToVertex * edgeDotSpherePositionToVertex);

			if (getLowestRoot(a, b, c, t, &newT)) {
				float f = (edgeDotVelocity * newT - edgeDotSpherePositionToVertex) / edgeLengthSquared;
				if (f >= 0.0f && f <= 1.0f) {
					// If the collision with the edge happened, we set the results
					t = newT;
					collidingWithTri = true;
					collisionPoint = _p0 + f * edge;
				}
			}

			// Edge (p1, p2):
			edge = _p2 - _p1;
			spherePositionToVertex = _p1 - position;
			edgeLengthSquared = XMVectorGetX(XMVector3Length(edge));
			edgeLengthSquared = edgeLengthSquared * edgeLengthSquared;
			edgeDotVelocity = XMVectorGetX(XMVector3Dot(edge, _cP.e_Velocity));
			edgeDotSpherePositionToVertex = XMVectorGetX(XMVector3Dot(edge, spherePositionToVertex));
			spherePositionToVertexLengthSquared = XMVectorGetX(XMVector3Length(spherePositionToVertex));
			spherePositionToVertexLengthSquared = spherePositionToVertexLengthSquared * spherePositionToVertexLengthSquared;

			a = edgeLengthSquared * -velocityLengthSquared + (edgeDotVelocity * edgeDotVelocity);
			b = edgeLengthSquared * (2.0f * XMVectorGetX(XMVector3Dot(velocity, spherePositionToVertex))) - (2.0f * edgeDotVelocity * edgeDotSpherePositionToVertex);
			c = edgeLengthSquared * (1.0f - spherePositionToVertexLengthSquared) + (edgeDotSpherePositionToVertex * edgeDotSpherePositionToVertex);

			if (getLowestRoot(a, b, c, t, &newT)) {
				float f = (edgeDotVelocity * newT - edgeDotSpherePositionToVertex) / edgeLengthSquared;
				if (f >= 0.0f && f <= 1.0f) {
					t = newT;
					collidingWithTri = true;
					collisionPoint = _p1 + f * edge;
				}
			}

			// Edge (p2, p0):
			edge = _p0 - _p2;
			spherePositionToVertex = _p2 - position;
			edgeLengthSquared = XMVectorGetX(XMVector3Length(edge));
			edgeLengthSquared = edgeLengthSquared * edgeLengthSquared;
			edgeDotVelocity = XMVectorGetX(XMVector3Dot(edge, velocity));
			edgeDotSpherePositionToVertex = XMVectorGetX(XMVector3Dot(edge, spherePositionToVertex));
			spherePositionToVertexLengthSquared = XMVectorGetX(XMVector3Length(spherePositionToVertex));
			spherePositionToVertexLengthSquared = spherePositionToVertexLengthSquared * spherePositionToVertexLengthSquared;

			a = edgeLengthSquared * -velocityLengthSquared + (edgeDotVelocity * edgeDotVelocity);
			b = edgeLengthSquared * (2.0f * XMVectorGetX(XMVector3Dot(velocity, spherePositionToVertex))) - (2.0f * edgeDotVelocity * edgeDotSpherePositionToVertex);
			c = edgeLengthSquared * (1.0f - spherePositionToVertexLengthSquared) + (edgeDotSpherePositionToVertex * edgeDotSpherePositionToVertex);

			if (getLowestRoot(a, b, c, t, &newT)) {
				float f = (edgeDotVelocity * newT - edgeDotSpherePositionToVertex) / edgeLengthSquared;
				if (f >= 0.0f && f <= 1.0f) {
					t = newT;
					collidingWithTri = true;
					collisionPoint = _p2 + f * edge;
				}
			}
		}

		if (collidingWithTri == true)
		{
			// Find the distance to the collision
			float distToCollision = t * XMVectorGetX(XMVector3Length(velocity));

			// check if this is the first triangle that has been collided with OR it is 
			// the closest triangle yet that was collided with
			if (_cP.foundCollision == false || distToCollision < _cP.nearestDistance) {

				// Collision response information
				_cP.nearestDistance = distToCollision;
				_cP.intersectionPoint = collisionPoint;

				_cP.foundCollision = true;
				return true;
			}
		}
	}
	return false;
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