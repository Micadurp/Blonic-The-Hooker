#ifndef PLAYER_H
#define PLAYER_H

#include "Camera.h"
#include "PlayerInputs.h";

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

		PlayerInputs* m_input;

		XMFLOAT4 m_defaultForward;
		XMFLOAT4 m_defaultRight;
		XMFLOAT4 m_currentForward;
		XMFLOAT4 m_currentRight;

		XMFLOAT3 m_gravity;
		XMFLOAT3 m_velocity;

	public:
		Player();
		virtual ~Player();

		bool Initialize(HWND &wndHandle, HINSTANCE &hInstance);

		virtual void Update(double time);
		void Move(XMFLOAT2* _movement, XMFLOAT2 _rotation);

	private:
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
};

#endif