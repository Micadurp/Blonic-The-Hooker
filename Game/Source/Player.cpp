#include "Player.h"

Player::Player()
{
	m_input = new PlayerInputs();

	m_crosshair = new Model();
	m_crosshair2 = new Model();

	m_position = { 0.0f, 0.0f };

	m_defaultForward = { 0.0f, 0.0f, 1.0f, 0.0f };
	m_defaultRight = { 1.0f, 0.0f, 0.0f, 0.0f };
	m_currentForward = { 0.0f, 0.0f, 1.0f, 0.0f };
	m_currentRight = { 1.0f, 0.0f, 0.0f, 0.0f };

	m_gravity = { 0.0f, 0.02f, 0.0f };


	m_velocity = { 0.0f, 0.0f, 0.0f };
	
	m_hookshot = new HookShot();
	m_hookshot->active = false;
	m_hookshot->point = { 0.0f, 0.0f, 0.0f };
	m_hookshot->velocity = { 0.0f, 0.0f, 0.0f};
	m_hookshot->length =  0;
	m_hookshot->maxLength = 100;

	//m_jumpVelocity = 0.0f;
	//m_isJumping = false;

	m_jumpPosition = { 0.0f, 0.0f, 0.0f, 0.0f };

	m_onGround = false;
	m_lookAtCrystal = false;

	hookString = new HookString();
}

Player::~Player()
{
}


bool Player::Initialize(HWND &wndHandle, HINSTANCE &hInstance, ID3D11Device* _device)
{
	bool result;

	result = m_input->Initialize(wndHandle, hInstance);
	if (!result)
	{
		return false;
	}

	result = m_crosshair->Initialize(L"crosshair_plane", _device);
	if (!result)
	{
		return false;
	}

	result = m_crosshair2->Initialize(L"crosshair2_plane", _device);
	if (!result)
	{
		return false;
	}

	result = hookString->Initialize(_device);
	if (!result)
	{
		return false;
	}

	m_crosshair->SetObjMatrix(XMMatrixScaling(0.15f, 0.2f, 0.15f) * XMMatrixTranslation(0, 0, 0));
	m_crosshair2->SetObjMatrix(XMMatrixScaling(0.15f, 0.2f, 0.15f) * XMMatrixTranslation(0, 0, 0));

	return true;
}

void Player::Shutdown()
{
	if (m_hookshot)
	{
		delete m_hookshot;
		m_hookshot = 0;
	}

	if (m_input)
	{
		delete m_input;
		m_input = 0;
	}

	if (m_crosshair)
	{
		m_crosshair->Shutdown();
		delete m_crosshair;
		m_crosshair = 0;
	}


	if (hookString)
	{
		hookString->Shutdown();
		delete hookString;
		hookString = 0;
	}

	if (m_crosshair2)
	{
		m_crosshair2->Shutdown();
		delete m_crosshair2;
		m_crosshair2 = 0;

	}
}


int Player::Update(double time, std::vector<XMFLOAT3> collidableGeometryPositions, std::vector<uint32_t> collidableGeometryIndices, vector<Model*> models)
{

	m_lastKeyboard = m_input->GetKeyboardState();

	// Check inputs
	m_input->Update();

	if (m_input->GetKeyboardState().key_escape_pressed && !m_lastKeyboard.key_escape_pressed)
	{
		return 1;
	}

	// Get input values
	m_keyboard = m_input->GetKeyboardState();
	m_mouse = m_input->GetMouseState();


	ChangeHookState(models, collidableGeometryPositions, collidableGeometryIndices);

	// Update camera position and rotation according to inputs
	Move(time, collidableGeometryPositions, collidableGeometryIndices);



	return -1;
}

void Player::Render(Direct3D * _direct3D)
{
	if (m_lookAtCrystal)
	{
		m_crosshair2->Render(_direct3D->GetDeviceContext());
	}
	else
	{
		m_crosshair->Render(_direct3D->GetDeviceContext());
	}
	m_lookAtCrystal = false;
}

void Player::RenderRope(Direct3D * _direct3D)
{

	if (hookString->GetActive())
	{
		hookString->Update(_direct3D, &XMLoadFloat4x4(&m_viewMatrix), &XMLoadFloat4(&m_camPos), &XMLoadFloat3(&m_hookshot->point));
		hookString->Render(_direct3D);
	}
}

XMFLOAT4X4 Player::GetCrosshairMatrix()
{
	XMFLOAT4X4 mat;
	XMStoreFloat4x4(&mat, m_crosshair->GetObjMatrix());

	return mat;
}


void Player::Move(double _time, std::vector<XMFLOAT3> collidableGeometryPositions, std::vector<uint32_t> collidableGeometryIndices)
{
	const float speed = 15.0f * (float)_time;

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

	if (m_keyboard.key_space_pressed && m_hookshot->active != 1 && m_onGround)
	{
		m_velocity.y = 0.5f;
	}
	else if (m_keyboard.key_space_pressed)
	{
		m_hookshot->active = 0;
	}

	// Rotation matrix from mouse input
	XMMATRIX cameraRotationMatrix = XMMatrixRotationRollPitchYaw(m_mouse.y_pos, m_mouse.x_pos, 0.0f);

	// Temporary convert to XMVECTOR
	XMVECTOR temp_camPos = XMVectorSet(m_camPos.x, m_camPos.y, m_camPos.z, m_camPos.w);

	// Align vectors and look at position according to mouse input
	XMVECTOR temp_camLook = XMVector3TransformCoord(XMLoadFloat4(&m_defaultForward), cameraRotationMatrix);
	temp_camLook = XMVector3Normalize(temp_camLook);

	/*XMStoreFloat4(&m_currentRight, XMVector3TransformCoord(XMLoadFloat4(&m_defaultRight), cameraRotationMatrix));
	XMStoreFloat4(&m_currentForward, XMVector3TransformCoord(XMLoadFloat4(&m_defaultForward), cameraRotationMatrix));*/

	//First person camera, doesnt move on y axis
	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(m_mouse.x_pos);

	XMStoreFloat4(&m_currentRight, XMVector3TransformCoord(XMLoadFloat4(&m_defaultRight), RotateYTempMatrix));
	XMStoreFloat4(&m_currentForward, XMVector3TransformCoord(XMLoadFloat4(&m_defaultForward), RotateYTempMatrix));

	XMVECTOR temp_camUp = XMVector3Cross(XMLoadFloat4(&m_currentForward), XMLoadFloat4(&m_currentRight));

	hookString->SetActive(false);

	if (m_hookshot->active == 1) // Using hookshot
	{
		hookString->SetActive(true);
		HookToObj(XMLoadFloat3(&m_hookshot->point));
		XMStoreFloat3(&m_velocity, XMLoadFloat3(&m_hookshot->velocity));
	}
	else if (m_hookshot->active == 2) //Using grappling hook
	{
		hookString->SetActive(true);

		if (!m_onGround) // Not on ground
		{
			// Set maximum falling velocity
			if (m_velocity.y > -2.0f)
			{
				m_velocity.y -= m_gravity.y;
			}

			GrappleToObj(XMLoadFloat3(&m_hookshot->point), XMLoadFloat3(&m_velocity));
			XMStoreFloat3(&m_velocity, XMLoadFloat3(&m_hookshot->velocity) * 0.998f);

		}
		else // On ground
		{
			if (m_velocity.y < 0.0f)
			{
				m_velocity.y = 0.0f;
			}

			GrappleToObj(XMLoadFloat3(&m_hookshot->point), XMLoadFloat3(&m_velocity));
			XMStoreFloat3(&m_velocity, XMLoadFloat3(&m_hookshot->velocity));
			XMStoreFloat3(&m_velocity, m_position.x * XMLoadFloat4(&m_currentRight) + m_position.y * XMLoadFloat4(&m_currentForward) + (m_velocity.y - m_gravity.y) * temp_camUp);
		}
	}
	else // Hookshots not active
	{
		if (!m_onGround) //Not on ground
		{
			m_velocity.y -= m_gravity.y;
			// Set maximum falling velocity
			if (m_velocity.y < -2.0f)
			{
				m_velocity.y = -2.0f;
			}
		}
		else //On ground
		{
			if (m_velocity.y < 0.0f)
			{
				m_velocity.y = 0.0f;
			}

			XMStoreFloat3(&m_velocity, m_position.x * XMLoadFloat4(&m_currentRight) + m_position.y * XMLoadFloat4(&m_currentForward) + (m_velocity.y - m_gravity.y) * temp_camUp);
		}
	}

	m_onGround = false;
	temp_camPos = Collision(collidableGeometryPositions, collidableGeometryIndices, XMLoadFloat3(&m_velocity));


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


XMVECTOR Player::Collision(vector<XMFLOAT3>& _vertPos, vector<uint32_t>& _indices, const XMVECTOR &velocity)
{
	CollisionPacket collisionPack;
	collisionPack.ellipsoidSpace = XMVectorSet(1.0f, 3.0f, 1.0f, 0.0f);
	collisionPack.w_Position = XMLoadFloat4(&m_camPos) + XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	collisionPack.w_Velocity = velocity;

	// Transform velocity vector to the ellipsoid space (e_ denotes ellipsoid space)
	collisionPack.e_Velocity = collisionPack.w_Velocity / collisionPack.ellipsoidSpace;

	// Transform position vector to the ellipsoid space
	collisionPack.e_Position = collisionPack.w_Position / collisionPack.ellipsoidSpace;

	// Now we check for a collision with our world
	collisionPack.collisionRecursionDepth = 0;
	XMVECTOR finalPosition = CollideWithWorld(collisionPack, _vertPos, _indices);


	//if (m_hookshot->active == 2)
	//{	
	//	GrappleToObj(m_hookshot->point, collisionPack.e_Velocity * collisionPack.ellipsoidSpace);
	//	collisionPack.e_Velocity = (m_hookshot->velocity * 0.999) / collisionPack.ellipsoidSpace;
	//	collisionPack.e_Position = finalPosition;
	//	collisionPack.collisionRecursionDepth = 0;
	//	finalPosition = CollideWithWorld(collisionPack, _vertPos, _indices);
	//}


	// Convert our final position from ellipsoid space to world space
	finalPosition = finalPosition * collisionPack.ellipsoidSpace + XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMStoreFloat3(&m_velocity, collisionPack.e_Velocity * collisionPack.ellipsoidSpace);
	return finalPosition;
}


XMVECTOR Player::CollideWithWorld(CollisionPacket& _cP, vector<XMFLOAT3>& _vertPos, vector<uint32_t>& _indices)
{
	const float veryCloseDistance = 0.005f;

	if (_cP.collisionRecursionDepth > 5)
		return _cP.e_Position;

	// Normalize velocity vector
	_cP.e_normalizedVelocity = XMVector3Normalize(_cP.e_Velocity);

	_cP.foundCollision = false;
	_cP.nearestDistance = 0.0f;

	// Loop through each triangle in mesh and check for a collision
	for (size_t triCounter = 0; triCounter < _indices.size() / 3; triCounter++)
	{
		XMVECTOR p0, p1, p2;
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
	if (_cP.foundCollision == false) 
	{
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
			b = 2.0f*(XMVectorGetX(XMVector3Dot(velocity, position - _p1)));
			c = XMVectorGetX(XMVector3Length((_p1 - position)));
			c = (c*c) - 1.0f;
			if (GetLowestRoot(a, b, c, t, &newT)) {
				t = newT;
				collidingWithTri = true;
				collisionPoint = _p1;
			}

			// P2 - Collision test with sphere and p2
			b = 2.0f*(XMVectorGetX(XMVector3Dot(velocity, position - _p2)));
			c = XMVectorGetX(XMVector3Length((_p2 - position)));
			c = (c*c) - 1.0f;
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

			if (XMVectorGetX(XMVector3AngleBetweenVectors(_triNormal, XMLoadFloat4(&m_camUp)))<XM_PIDIV4)
			{
				m_onGround = true;
			}

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
bool Player::GetLowestRoot(const float _a, const float _b, const float _c, const float _maxR, float * _root)
{
	// Check if a solution exists
	const float determinant = _b*_b - 4.0f*_a*_c;
	// If determinant is negative it means no solutions.
	if (determinant < 0.0f) return false;
	// calculate the two roots
	const float sqrtD = sqrt(determinant);
	float r1 = (-_b - sqrtD) / (2 * _a);
	float r2 = (-_b + sqrtD) / (2 * _a);
	// Sort so x1 <= x2
	if (r1 > r2) {
		const float temp = r2;
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

void Player::ChangeHookState(const std::vector<Model*> models, const std::vector<XMFLOAT3> collidableGeometryPositions, const std::vector<uint32_t> collidableGeometryIndices)
{
	const MouseStateStruct mousestate = m_input->GetMouseState();
	Triangle tri;
	XMVECTOR point;
	float shortestLength = 1000;
	const size_t modelSize = models.size();

	//look at crystal check + blocked by stuff check
	for (size_t n = 1; n < modelSize-1; n++)
	{
		const std::vector<XMFLOAT3> const * pickingPoints = models.at(n)->GetPickingPoints();
		const std::vector<uint32_t> const * pickingIndicies = models.at(n)->GetPickingIndicies();
		const size_t pickingIndiciesSize = pickingIndicies->size();
		for (size_t i = 0; i < pickingIndiciesSize; i += 3)
		{
			tri.vertex0 = XMLoadFloat3(&pickingPoints->at(pickingIndicies->at(i)));
			tri.vertex1 = XMLoadFloat3(&pickingPoints->at(pickingIndicies->at(i + 1)));
			tri.vertex2 = XMLoadFloat3(&pickingPoints->at(pickingIndicies->at(i + 2)));
			if (TestIntersection(tri, &point, models.at(n)->GetObjMatrix()))
			{
				const XMVECTOR t = XMLoadFloat4(&m_camPos) - point;
				float length = sqrt(pow(XMVectorGetX(t), 2) + pow(XMVectorGetY(t), 2) + pow(XMVectorGetZ(t), 2));
				if (shortestLength > length)
				{
					shortestLength = length;
				}

				
			}
		}
	}
	
	if (shortestLength <= m_hookshot->maxLength)
	{
		m_lookAtCrystal = true;

		const size_t collidableGeometryIndicesSize = collidableGeometryIndices.size();
		for (size_t i = 0; i < collidableGeometryIndicesSize; i += 3)
		{
			tri.vertex0 = XMLoadFloat3(&collidableGeometryPositions.at(collidableGeometryIndices.at(i)));
			tri.vertex1 = XMLoadFloat3(&collidableGeometryPositions.at(collidableGeometryIndices.at(i + 1)));
			tri.vertex2 = XMLoadFloat3(&collidableGeometryPositions.at(collidableGeometryIndices.at(i + 2)));
			if (TestIntersection(tri, &point, XMMatrixIdentity()))
			{
				const XMVECTOR b = XMLoadFloat4(&m_camPos) - point;
				float block = sqrt(pow(XMVectorGetX(b), 2) + pow(XMVectorGetY(b), 2) + pow(XMVectorGetZ(b), 2));
				if (shortestLength > block)
				{
					m_lookAtCrystal = false;
				}
			}
		}
	}

	if (!m_lastpick && mousestate.btn_left_pressed)
	{
		if (CheckHookState())
		{
			TurnOffHookShot();
		}
		if (m_lookAtCrystal)
		{
			for (size_t n = 1; n < modelSize-1; n++)
			{
				const std::vector<XMFLOAT3> const * pickingPoints = models.at(n)->GetPickingPoints();
				const std::vector<uint32_t> const * pickingIndicies = models.at(n)->GetPickingIndicies();

				const size_t pickingIndiciesSize = pickingIndicies->size();
				for (size_t i = 0; i < pickingIndiciesSize; i += 3)
				{
					tri.vertex0 = XMLoadFloat3(&pickingPoints->at(pickingIndicies->at(i)));
					tri.vertex1 = XMLoadFloat3(&pickingPoints->at(pickingIndicies->at(i + 1)));
					tri.vertex2 = XMLoadFloat3(&pickingPoints->at(pickingIndicies->at(i + 2)));
					if (TestIntersection(tri, &point, models.at(n)->GetObjMatrix()))
					{
						const XMVECTOR t = XMLoadFloat4(&m_camPos) - point;
						m_hookshot->length = sqrt(pow(XMVectorGetX(t), 2) + pow(XMVectorGetY(t), 2) + pow(XMVectorGetZ(t), 2));
						if (m_hookshot->length <= m_hookshot->maxLength)
						{
							HookToObj(point);
						}
					}
				}
			}
		}
	}

	if (!m_lastpick && mousestate.btn_right_pressed)
	{
		if (CheckHookState())
		{
			TurnOffHookShot();
		}
		if (m_lookAtCrystal)
		{
			for (size_t n = 1; n < modelSize-1; n++)
			{
				const std::vector<XMFLOAT3> const * pickingPoints = models.at(n)->GetPickingPoints();
				const std::vector<uint32_t> const * pickingIndicies = models.at(n)->GetPickingIndicies();

				const size_t pickingIndiciesSize = pickingIndicies->size();
				for (size_t i = 0; i < pickingIndiciesSize; i += 3)
				{
					tri.vertex0 = XMLoadFloat3(&pickingPoints->at(pickingIndicies->at(i)));
					tri.vertex1 = XMLoadFloat3(&pickingPoints->at(pickingIndicies->at(i + 1)));
					tri.vertex2 = XMLoadFloat3(&pickingPoints->at(pickingIndicies->at(i + 2)));
					if (TestIntersection(tri, &point, models.at(n)->GetObjMatrix()))
					{
						const XMVECTOR t = XMLoadFloat4(&m_camPos) - point;
						m_hookshot->length = (sqrt(pow(XMVectorGetX(t), 2) + pow(XMVectorGetY(t), 2) + pow(XMVectorGetZ(t), 2))) * 0.6f;
						if (m_hookshot->length <= m_hookshot->maxLength)
						{
							GrappleToObj(point, XMLoadFloat3(&m_velocity));
						}
					}
				}
			}
		}
	}

	if (mousestate.btn_right_pressed)
		m_lastpick = mousestate.btn_right_pressed;
	else
		m_lastpick = mousestate.btn_left_pressed;

}


void Player::HookToObj(const XMVECTOR &point)
{
	XMVECTOR vec = point - XMLoadFloat4(&m_camPos);
	vec = XMVector3Normalize(vec);
	XMStoreFloat3(&m_hookshot->velocity, vec);
	XMStoreFloat3(&m_hookshot->point, point);
	m_hookshot->active = 1;
}

void Player::GrappleToObj(const XMVECTOR &point, XMVECTOR &velocity)
{
	XMVECTOR t = XMLoadFloat4(&m_camPos) - point;

	const float dist = sqrt(pow(XMVectorGetX(t), 2) + pow(XMVectorGetY(t), 2) + pow(XMVectorGetZ(t), 2));
	if (dist > m_hookshot->length)
	{
		//mVelocity += (1000 * moveVec / mMass) * TickSec;


		t = XMVector3Normalize(t);

		float mag;
		mag = sqrt((XMVectorGetX(velocity) * XMVectorGetX(velocity)) + (XMVectorGetY(velocity) * XMVectorGetY(velocity)) + (XMVectorGetZ(velocity) * XMVectorGetZ(velocity)));

		if (mag != 0)
		{
			XMVECTOR val = XMVectorSet(1.57f, 1.57f, 1.57f, 1.57f);
			if (XMVector3Less(XMVectorACos(XMVector3Dot(t, velocity) / mag), val))
			{
				velocity -= (XMVector3Dot(t, velocity) / XMVector3Dot(t, t)) * t;
				velocity = XMVector3Normalize(velocity) * mag;
			}
		}
	}

	XMStoreFloat3(&m_hookshot->velocity, velocity);
	XMStoreFloat3(&m_hookshot->point, point);
	m_hookshot->active = 2;
}

void Player::TurnOffHookShot()
{
	m_hookshot->active = false;
	m_velocity = m_hookshot->velocity;
}

bool Player::CheckHookState()
{
	if (m_hookshot->active != 0)
	{
		return true;
	}
	return false;
}

bool Player::TestIntersection(const Triangle & tri, XMVECTOR * point, const XMMATRIX & objMatrix)
{
	XMMATRIX inverseWorldMatrix;
	XMVECTOR inverseView;
	XMFLOAT4 direction, origin;
	XMVECTOR rayOrigin, rayDirection;
	bool intersect;

	// Get the inverse of the view matrix.
	inverseView = XMMatrixInverse(NULL, XMLoadFloat4x4(&m_viewMatrix)).r[2];

	// Calculate the direction of the picking ray in view space.
	direction.x = XMVectorGetX(inverseView);
	direction.y = XMVectorGetY(inverseView);
	direction.z = XMVectorGetZ(inverseView);

	// Get the origin of the picking ray which is the position of the camera.
	origin = m_camPos;

	// Now get the inverse of the translated world matrix.
	inverseWorldMatrix = XMMatrixInverse(NULL, objMatrix);

	// Now transform the ray origin and the ray direction from view space to world space.
	rayOrigin = XMVector3TransformCoord(XMLoadFloat4(&origin), inverseWorldMatrix);
	rayDirection = XMVector3TransformNormal(XMLoadFloat4(&direction), inverseWorldMatrix);

	// Normalize the ray direction.
	rayDirection = XMVector3Normalize(rayDirection);

	//Test intersection between the ray and the triangle
	float distance;
	intersect = DirectX::TriangleTests::Intersects(rayOrigin, rayDirection, tri.vertex0, tri.vertex1, tri.vertex2, distance);
	*point = rayOrigin + distance * rayDirection;

	return intersect;
}

bool Player::IsDead()
{
	if (m_camPos.y < -300)
	{
		return true;
	}
	return false;
}

bool Player::Win(Model * winZone)
{
	bool victory = false;
	XMVECTOR t;

	for (size_t i = 0; i < winZone->GetPickingPoints()->size(); ++i)
	{
		t = XMLoadFloat4(&m_camPos) - XMLoadFloat3(&winZone->GetPickingPoints()->at(i));
		const float dist = sqrt(pow(XMVectorGetX(t), 2) + pow(XMVectorGetY(t), 2) + pow(XMVectorGetZ(t), 2));
		
		if (dist < 4.0f)
		{
			victory = true;
		}
	}
	return victory;
}