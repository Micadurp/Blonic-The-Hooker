#ifndef PLAYER_H
#define PLAYER_H

#include "Camera.h"
#include "PlayerInputs.h";
#include "Model.h";

using namespace std;

class Player : public Camera
{
	private:
		struct CollisionPacket{
			// Information about ellipsoid (in world space)
			XMVECTOR ellipsoidSpace;
			XMVECTOR w_Position;
			XMVECTOR w_Velocity;

			// Information about ellipsoid (in ellipsoid space)
			XMVECTOR e_Position;
			XMVECTOR e_Velocity;
			XMVECTOR e_normalizedVelocity;

			// Collision Information
			bool foundCollision;
			float nearestDistance;
			XMVECTOR intersectionPoint;
			int collisionRecursionDepth;
		};

		struct HookShot
		{
			XMVECTOR velocity;
			XMMATRIX object;
			bool active;
		};

		Model* m_crosshair;

		PlayerInputs* m_input;

		KeyboardStateStruct m_keyboard;
		MouseStateStruct m_mouse;

		XMFLOAT2 m_position;

		XMFLOAT4 m_defaultForward;
		XMFLOAT4 m_defaultRight;
		XMFLOAT4 m_currentForward;
		XMFLOAT4 m_currentRight;

		XMFLOAT3 m_gravity;
		XMFLOAT3 m_velocity;
		HookShot* m_hookshot;

		bool m_lastpick;

	public:
		Player();
		virtual ~Player();

		bool Initialize(HWND &_wndHandle, HINSTANCE &_hInstance, ID3D11Device* _device);

		void Update(double time, vector<XMFLOAT3> collidableGeometryPositions, std::vector<DWORD> collidableGeometryIndices);
		void Move(double time, vector<XMFLOAT3> collidableGeometryPositions, vector<DWORD> collidableGeometryIndices);

		void Render(ID3D11DeviceContext* _deviceContext);

		XMFLOAT4X4 GetCrosshairMatrix();

		void ChangeHookState(vector<Model*> models);

	private:
		void Move(double time);

		// Collision Detection and Response Function Prototypes
		XMVECTOR Collision(vector<XMFLOAT3>& vertPos,		// An array holding the polygon soup vertex positions
			vector<DWORD>& indices);						// An array holding the polygon soup indices (triangles)

		XMVECTOR CollideWithWorld(CollisionPacket& cP,			// Same arguments as the above function
			vector<XMFLOAT3>& vertPos,
			vector<DWORD>& indices);

		bool SphereCollidingWithTriangle(CollisionPacket& cP,	// Pointer to a CollisionPacket object	
			XMVECTOR &p0,										// First vertex position of triangle
			XMVECTOR &p1,										// Second vertex position of triangle
			XMVECTOR &p2,										// Third vertex position of triangle 
			XMVECTOR &triNormal);								// Triangle's Normal

		// Checks if a point (inside the triangle's plane) is inside the triangle
		bool CheckPointInTriangle(const XMVECTOR& point, const XMVECTOR& triV1, const XMVECTOR& triV2, const XMVECTOR& triV3);

		// Solves the quadratic eqation, and returns the lowest root if equation is solvable, returns false if not solvable
		bool GetLowestRoot(float a, float b, float c, float maxR, float* root); 

		void MoveTowards(const XMMATRIX &object);
		void TurnOffHookShot();
		bool CheckHookState();

		bool TestIntersection(Model* obj);
		bool RaySphereIntersect(XMVECTOR rayOrigin, XMVECTOR rayDirection, float radius);
};

#endif