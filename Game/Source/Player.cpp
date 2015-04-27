#include "Player.h"

Player::Player()
{
	m_input = new PlayerInputs();

	m_position = { 0.0f, 0.0f };

	m_defaultForward = { 0.0f, 0.0f, 1.0f, 0.0f };
	m_defaultRight = { 1.0f, 0.0f, 0.0f, 0.0f };
	m_currentForward = { 0.0f, 0.0f, 1.0f, 0.0f };
	m_currentRight = { 1.0f, 0.0f, 0.0f, 0.0f };

	m_gravity = { 0.0f, 9.806f, 0.0f };

	m_velocity = { 0.0f, 0.0f, 0.0f };
}

Player::~Player()
{
}


bool Player::Initialize(HWND &wndHandle, HINSTANCE &hInstance)
{
	bool result;

	result = m_input->Initialize(wndHandle, hInstance);
	if (!result)
	{
		return false;
	}

	return true;
}


void Player::Update(double time)
{
	// Check inputs
	m_input->Update();

	// Get input values
	m_keyboard = m_input->GetKeyboardState();
	m_mouse = m_input->GetMouseState();

	// Update camera position and rotation accordingly
	Move(time);
}


void Player::Move(double time)
{
	float speed = 10.0f * time;

	// Change camera position according to keyboard inputs
	if (m_keyboard.key_a_pressed)
	{
		m_position.x -= speed;
	}
	if (m_keyboard.key_d_pressed)
	{
		m_position.x += speed;
	}
	if (m_keyboard.key_w_pressed)
	{
		m_position.y += speed;
	}
	if (m_keyboard.key_s_pressed)
	{
		m_position.y -= speed;
	}


	// Rotation matrix from mouse input
	XMMATRIX cameraRotationMatrix = XMMatrixRotationRollPitchYaw(m_mouse.y_pos, m_mouse.x_pos, 0.0f);

	// Temporary convert to XMVECTOR
	XMVECTOR temp_camPos = XMVectorSet(m_camPos.x, m_camPos.y, m_camPos.z, m_camPos.w);
	XMVECTOR temp_camLook = XMVectorSet(m_camLook.x, m_camLook.y, m_camLook.z, m_camLook.w);
	XMVECTOR temp_camUp = XMVectorSet(m_camUp.x, m_camUp.y, m_camUp.z, m_camUp.w);

	// Align vectors and look at position according to mouse input
	temp_camLook = XMVector3TransformCoord(XMLoadFloat4(&m_defaultForward), cameraRotationMatrix);
	temp_camLook = XMVector3Normalize(temp_camLook);

	XMStoreFloat4(&m_currentRight, XMVector3TransformCoord(XMLoadFloat4(&m_defaultRight), cameraRotationMatrix));
	XMStoreFloat4(&m_currentForward, XMVector3TransformCoord(XMLoadFloat4(&m_defaultForward), cameraRotationMatrix));

	temp_camUp = XMVector3Cross(XMLoadFloat4(&m_currentForward), XMLoadFloat4(&m_currentRight));

	// New position from keyboard inputs
	temp_camPos += m_position.x * XMLoadFloat4(&m_currentRight);
	temp_camPos += m_position.y * XMLoadFloat4(&m_currentForward);

	// Reset input movement variable
	m_position.x = 0.0f;
	m_position.y = 0.0f;

	// Position look at accordingly to keyboard input
	temp_camLook += temp_camPos;

	// Store temporary values back in member variables
	XMStoreFloat4(&m_camPos, temp_camPos);
	XMStoreFloat4(&m_camLook, temp_camLook);
	XMStoreFloat4(&m_camUp, temp_camUp);


	// Update camera view
	UpdateViewMatrix(m_camPos, m_camLook, m_camUp);
}


XMVECTOR Player::Collision(vector<XMFLOAT3>& _vertPos, vector<DWORD>& _indices)
{
	CollisionPacket collisionPack;
	collisionPack.ellipsoidSpace = XMVectorSet(1.0f, 3.0f, 1.0f, 0.0f);
	collisionPack.w_Position = XMLoadFloat4(&m_camPos);
	collisionPack.w_Velocity = XMLoadFloat3(&m_velocity);

	// Transform velocity vector to the ellipsoid space (e_ denotes ellipsoid space)
	collisionPack.e_Velocity = collisionPack.w_Velocity / collisionPack.ellipsoidSpace;

	// Transform position vector to the ellipsoid space
	collisionPack.e_Position = collisionPack.w_Position / collisionPack.ellipsoidSpace;

	// Now we check for a collision with our world
	collisionPack.collisionRecursionDepth = 0;
	XMVECTOR finalPosition = CollideWithWorld(collisionPack, _vertPos, _indices);

	//// Add gravity pull
	collisionPack.e_Velocity = XMLoadFloat3(&m_gravity) / collisionPack.ellipsoidSpace;
	collisionPack.e_Position = finalPosition;
	collisionPack.collisionRecursionDepth = 0;
	finalPosition = CollideWithWorld(collisionPack, _vertPos, _indices);

	// Convert our final position from ellipsoid space to world space
	finalPosition = finalPosition * collisionPack.ellipsoidSpace;

	return finalPosition;
}


XMVECTOR Player::CollideWithWorld(CollisionPacket& _cP, vector<XMFLOAT3>& _vertPos, vector<DWORD>& _indices)
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


bool Player::SphereCollidingWithTriangle(CollisionPacket& _cP, XMVECTOR &_p0, XMVECTOR &_p1, XMVECTOR &_p2, XMVECTOR &_triNormal)
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
			if (CheckPointInTriangle(planeIntersectionPoint, _p0, _p1, _p2))
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
			if (GetLowestRoot(a, b, c, t, &newT)) {	// Check if the equation can be solved
				t = newT;
				collidingWithTri = true;
				collisionPoint = _p0;
			}

			// P1 - Collision test with sphere and p1
			b = 2.0*(XMVectorGetX(XMVector3Dot(velocity, position - _p1)));
			c = XMVectorGetX(XMVector3Length((_p1 - position)));
			c = (c*c) - 1.0;
			if (GetLowestRoot(a, b, c, t, &newT)) {
				t = newT;
				collidingWithTri = true;
				collisionPoint = _p1;
			}

			// P2 - Collision test with sphere and p2
			b = 2.0*(XMVectorGetX(XMVector3Dot(velocity, position - _p2)));
			c = XMVectorGetX(XMVector3Length((_p2 - position)));
			c = (c*c) - 1.0;
			if (GetLowestRoot(a, b, c, t, &newT)) {
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

			if (GetLowestRoot(a, b, c, t, &newT)) {
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

			if (GetLowestRoot(a, b, c, t, &newT)) {
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

			if (GetLowestRoot(a, b, c, t, &newT)) {
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


bool Player::CheckPointInTriangle(const XMVECTOR& _point, const XMVECTOR& _triV1, const XMVECTOR& _triV2, const XMVECTOR& _triV3)
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
bool Player::GetLowestRoot(float _a, float _b, float _c, float _maxR, float* _root)
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