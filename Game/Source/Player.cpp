#include "Player.h"

Player::Player()
{
	m_input = new PlayerInputs();

	m_crosshair = new Model();

	m_position = { 0.0f, 0.0f };

	m_defaultForward = { 0.0f, 0.0f, 1.0f, 0.0f };
	m_defaultRight = { 1.0f, 0.0f, 0.0f, 0.0f };
	m_currentForward = { 0.0f, 0.0f, 1.0f, 0.0f };
	m_currentRight = { 1.0f, 0.0f, 0.0f, 0.0f };

	m_gravity = { 0.0f, 0.01f, 0.0f };


	m_velocity = { 0.0f, 0.0f, 0.0f };
	
	m_hookshot = new HookShot();
	m_hookshot->active = false;
	m_hookshot->point = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	m_hookshot->velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	m_jumpVelocity = 0.0f;
	m_isjumping = false;

	m_jumpPosition = { 0.0f, 0.0f, 0.0f, 0.0f };
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

	m_crosshair->SetObjMatrix(XMMatrixScaling(0.05f, 0.065f, 0.05f) * XMMatrixTranslation(0, 0, 0));

	return true;
}


void Player::Update(double time, std::vector<XMFLOAT3> collidableGeometryPositions, std::vector<DWORD> collidableGeometryIndices)
{
	// Check inputs
	m_input->Update();

	// Get input values
	m_keyboard = m_input->GetKeyboardState();
	m_mouse = m_input->GetMouseState();

	// Update camera position and rotation according to inputs
	Move(time, collidableGeometryPositions, collidableGeometryIndices);
}

void Player::Render(ID3D11DeviceContext* _deviceContext)
{
	m_crosshair->Render(_deviceContext);
}

XMFLOAT4X4 Player::GetCrosshairMatrix()
{
	XMFLOAT4X4 mat;
	XMStoreFloat4x4(&mat, m_crosshair->GetObjMatrix());

	return mat;
}


void Player::Move(double _time, std::vector<XMFLOAT3> collidableGeometryPositions, std::vector<DWORD> collidableGeometryIndices)
{
	float speed = 15.0f * (float)_time;

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

	if (m_keyboard.key_space_pressed && m_hookshot->active == 0)
	{
		m_jumpVelocity = 1.0f;
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

	/*XMStoreFloat4(&m_currentRight, XMVector3TransformCoord(XMLoadFloat4(&m_defaultRight), cameraRotationMatrix));
	XMStoreFloat4(&m_currentForward, XMVector3TransformCoord(XMLoadFloat4(&m_defaultForward), cameraRotationMatrix));*/

	//First person camera, doesnt move on y axis
	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(m_mouse.x_pos);

	XMStoreFloat4(&m_currentRight, XMVector3TransformCoord(XMLoadFloat4(&m_defaultRight), RotateYTempMatrix));
	XMStoreFloat4(&m_currentForward, XMVector3TransformCoord(XMLoadFloat4(&m_defaultForward), RotateYTempMatrix));

	temp_camUp = XMVector3Cross(XMLoadFloat4(&m_currentForward), XMLoadFloat4(&m_currentRight));

	if (m_hookshot->active == 1)
	{
		m_jumpVelocity = 0.0f;
		HookToObj(m_hookshot->point);
		XMStoreFloat3(&m_velocity, m_hookshot->velocity);
	}
	else if (m_hookshot->active == 2)
	{
		GrappleToObj(m_hookshot->point);
		XMStoreFloat3(&m_velocity, m_hookshot->velocity);
	}
	else
	{
		XMStoreFloat3(&m_velocity, m_position.x * XMLoadFloat4(&m_currentRight) + m_position.y * XMLoadFloat4(&m_currentForward) + m_jumpVelocity * temp_camUp);

		m_jumpVelocity -= m_gravity.y;

		// Set maximum falling velocity
		if (m_jumpVelocity <= -2.001f)
		{
			m_jumpVelocity = -2.0f;
		}
	}

	temp_camPos = Collision(collidableGeometryPositions, collidableGeometryIndices);


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


	//if (!m_hookshot->active)
	//{	//// Add gravity pull
	//	collisionPack.e_Velocity = -XMLoadFloat3(&m_gravity) / collisionPack.ellipsoidSpace;
	//	collisionPack.e_Position = finalPosition;
	//	collisionPack.collisionRecursionDepth = 0;
	//	finalPosition = CollideWithWorld(collisionPack, _vertPos, _indices);
	//}

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

void Player::ChangeHookState(vector<Model*> models)
{
	MouseStateStruct mousestate = m_input->GetMouseState();
	Triangle tri;
	XMVECTOR point = XMVectorSet(0,0,0,0);

	if (!m_lastpick && mousestate.btn_left_pressed)
	{
		if (CheckHookState())
		{
			TurnOffHookShot();
		}
		for (int n = 1; n < models.size(); n++)
		{
			for (int i = 0; i < models.at(n)->GetPickingIndicies()->size(); i += 3)
			{
				tri.vertex0 = XMLoadFloat3(&models.at(n)->GetPickingPoints()->at(models.at(n)->GetPickingIndicies()->at(i)));
				tri.vertex1 = XMLoadFloat3(&models.at(n)->GetPickingPoints()->at(models.at(n)->GetPickingIndicies()->at(i + 1)));
				tri.vertex2 = XMLoadFloat3(&models.at(n)->GetPickingPoints()->at(models.at(n)->GetPickingIndicies()->at(i + 2)));
				if (TestIntersection(tri, point, models.at(n)->GetObjMatrix()))
				{
					HookToObj(point);
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
		for (int n = 1; n < models.size(); n++)
		{
			for (int i = 0; i < models.at(n)->GetPickingIndicies()->size(); i += 3)
			{
				tri.vertex0 = XMLoadFloat3(&models.at(n)->GetPickingPoints()->at(models.at(n)->GetPickingIndicies()->at(i)));
				tri.vertex1 = XMLoadFloat3(&models.at(n)->GetPickingPoints()->at(models.at(n)->GetPickingIndicies()->at(i + 1)));
				tri.vertex2 = XMLoadFloat3(&models.at(n)->GetPickingPoints()->at(models.at(n)->GetPickingIndicies()->at(i + 2)));
				if (TestIntersection(tri, point, models.at(n)->GetObjMatrix()))
				{
					GrappleToObj(point);
				}
			}
		}
	}
	m_lastpick = mousestate.btn_left_pressed;

}


void Player::HookToObj(const XMVECTOR &point)
{
	XMVECTOR vec = point - XMLoadFloat4(&m_camPos);
	vec = XMVector3Normalize(vec)/5;
	m_hookshot->velocity = vec;
	m_hookshot->point = point;
	m_hookshot->active = 1;
}

void Player::GrappleToObj(const XMVECTOR &point)
{
	XMVECTOR t = XMLoadFloat4(&m_camPos) - point;

	float r = sqrt(pow(XMVectorGetX(t), 2) + pow(XMVectorGetY(t), 2) + pow(XMVectorGetZ(t), 2));

	t = XMVector3Normalize(t);

	float mag;
	mag = sqrt((m_velocity.x * m_velocity.x) + (m_velocity.y * m_velocity.y) + (m_velocity.z * m_velocity.z));

	if (mag != 0)
	{
		//if (XMVector3Less(XMVectorACos(XMVector3Dot(t, XMLoadFloat3(&m_velocity)) / mag), 1.57))
		{
			XMStoreFloat3(&m_velocity, XMLoadFloat3(&m_velocity) - (XMVector3Dot(t, XMLoadFloat3(&m_velocity)) / XMVector3Dot(t, t)) * t);
			XMStoreFloat3(&m_velocity, (XMVector3Normalize(XMLoadFloat3(&m_velocity)) * mag));
		}
	}

	m_hookshot->velocity = XMLoadFloat3(&m_velocity);
	m_hookshot->point = point;
	m_hookshot->active = 2;
}

void Player::TurnOffHookShot()
{
	m_hookshot->active = false;
}

bool Player::CheckHookState()
{
	return m_hookshot->active;
}

bool Player::TestIntersection(Model* _obj)
{
	XMMATRIX inverseWorldMatrix;
	XMVECTOR inverseView;
	XMFLOAT4 direction, origin;
	XMVECTOR rayOrigin, rayDirection;
	bool intersect, result;
	intersect = false;

	// Get the inverse of the view matrix.
	inverseView = XMMatrixInverse(NULL, XMLoadFloat4x4(&m_viewMatrix)).r[2];

	// Calculate the direction of the picking ray in view space.
	direction.x = XMVectorGetX(inverseView);
	direction.y = XMVectorGetY(inverseView);
	direction.z = XMVectorGetZ(inverseView);

	// Get the origin of the picking ray which is the position of the camera.
	origin = m_camPos;

	// Now get the inverse of the translated world matrix.
	inverseWorldMatrix = XMMatrixInverse(NULL, _obj->GetObjMatrix());

	// Now transform the ray origin and the ray direction from view space to world space.
	rayOrigin = XMVector3TransformCoord(XMLoadFloat4(&origin), inverseWorldMatrix);
	rayDirection = XMVector3TransformNormal(XMLoadFloat4(&direction), inverseWorldMatrix);

	// Normalize the ray direction.
	rayDirection = XMVector3Normalize(rayDirection);

	// Now perform the ray-sphere intersection test.
	intersect = RaySphereIntersect(rayOrigin, rayDirection, 1.0f);

	return intersect;
}

bool Player::TestIntersection(const Triangle & tri, XMVECTOR & point, const XMMATRIX & objMatrix)
{
	XMMATRIX inverseWorldMatrix;
	XMVECTOR inverseView;
	XMFLOAT4 direction, origin;
	XMVECTOR rayOrigin, rayDirection;
	bool intersect, result;
	intersect = false;

	// Get the inverse of the view matrix.
	inverseView = XMLoadFloat4x4(&m_viewMatrix).r[2];

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

	Ray ray;
	ray.point0 = rayOrigin;
	ray.point1 = rayDirection;

	// Now perform the ray-sphere intersection test.
	intersect = RayTriangleIntersect(ray, tri, point);

	return intersect;
}

bool Player::RaySphereIntersect(XMVECTOR _rayOrigin, XMVECTOR _rayDirection, float _radius)
{
	float a, b, c, discriminant;
	XMFLOAT3 rayOrigin, rayDirection;

	DirectX::XMStoreFloat3(&rayOrigin, _rayOrigin);
	DirectX::XMStoreFloat3(&rayDirection, _rayDirection);

	// Calculate the a, b, and c coefficients.
	a = (rayDirection.x * rayDirection.x) + (rayDirection.y * rayDirection.y) + (rayDirection.z * rayDirection.z);
	b = ((rayDirection.x * rayOrigin.x) + (rayDirection.y * rayOrigin.y) + (rayDirection.z * rayOrigin.z)) * 2.0f;
	c = ((rayOrigin.x * rayOrigin.x) + (rayOrigin.y * rayOrigin.y) + (rayOrigin.z * rayOrigin.z)) - (_radius * _radius);

	// Find the discriminant.
	discriminant = (b * b) - (4 * a * c);

	// if discriminant is negative the picking ray missed the sphere, otherwise it intersected the sphere.
	if (discriminant < 0.0f)
	{
		return false;
	}

	return true;
}

bool Player::RayTriangleIntersect(const Ray & ray, const Triangle & tri, XMVECTOR & point)
{
	XMVECTOR  u, v, n;              // triangle vectors
	XMVECTOR  dir, w0, w;           // ray vectors
	float     r, a, b;              // params to calc ray-plane intersect

	// get triangle edge vectors and plane normal
	u = tri.vertex1 - tri.vertex0;
	v = tri.vertex2 - tri.vertex0;
	n = u * v;              // cross product

	dir = ray.point1 - ray.point0;              // ray direction vector
	w0 = ray.point0 - tri.vertex0;
	XMStoreFloat(&a, -XMVector3Dot(n, w0));
	XMStoreFloat(&b, XMVector3Dot(n, dir));
	if (fabs(b) <= 0) {     // ray is  parallel to triangle plane
		if (a == 0)                 // ray lies in triangle plane
			return true;
		else return false;              // ray disjoint from plane
	}

	// get intersect point of ray with triangle plane
	r = a / b;
	if (r < 0.0)                    // ray goes away from triangle
		if (a == 0)                 // ray lies in triangle plane
			return true;
		else return false;                   // => no intersect
	// for a segment, also test if (r > 1.0) => no intersect

	point = ray.point0 + r * dir;            // intersect point of ray and plane

	// is I inside T?
	float    uu, uv, vv, wu, wv, D;
	XMStoreFloat(&uu, XMVector3Dot(u, u));
	XMStoreFloat(&uv, XMVector3Dot(u, v));
	XMStoreFloat(&vv, XMVector3Dot(v, v));
	w = point - tri.vertex0;
	XMStoreFloat(&wu, XMVector3Dot(w, u));
	XMStoreFloat(&wv, XMVector3Dot(w, v));
	D = uv * uv - uu * vv;

	// get and test parametric coords
	float s, t;
	s = (uv * wv - vv * wu) / D;
	if (s < 0.0 || s > 1.0)         // I is outside T
		return false;
	t = (uv * wu - uu * wv) / D;
	if (t < 0.0 || (s + t) > 1.0)  // I is outside T
		return false;

	return true;                       // I is in T
}