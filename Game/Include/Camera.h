#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

class Camera
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

	XMFLOAT4X4 m_viewMatrix;

	XMVECTOR m_defaultForward;
	XMVECTOR m_defaultRight;
	XMVECTOR m_cameraForward;
	XMVECTOR m_cameraRight;

	XMVECTOR m_camPos;
	XMVECTOR m_camLook;
	XMVECTOR m_camUp;

	XMFLOAT3 gravity;
	XMFLOAT3 velocity;

public:
	Camera();
	~Camera();

	void Move(XMFLOAT2* _movement, XMFLOAT2 _rotation);

	void SetPosition(float _x, float _y, float _z);
	XMFLOAT3 GetPosition();

	void Update();
	XMMATRIX GetViewMatrix();

	XMFLOAT3 getCamLook();


	// Collision Detection and Response Function Prototypes
	XMVECTOR Collision(std::vector<XMFLOAT3>& vertPos,		// An array holding the polygon soup vertex positions
		std::vector<DWORD>& indices);						// An array holding the polygon soup indices (triangles)

	XMVECTOR CollideWithWorld(CollisionPacket& cP,			// Same arguments as the above function
		std::vector<XMFLOAT3>& vertPos,
		std::vector<DWORD>& indices);

	bool SphereCollidingWithTriangle(CollisionPacket& cP,	// Pointer to a CollisionPacket object	
		XMVECTOR &p0,										// First vertex position of triangle
		XMVECTOR &p1,										// Second vertex position of triangle
		XMVECTOR &p2,										// Third vertex position of triangle 
		XMVECTOR &triNormal);								// Triangle's Normal

	// Checks if a point (inside the triangle's plane) is inside the triangle
	bool checkPointInTriangle(const XMVECTOR& point, const XMVECTOR& triV1, const XMVECTOR& triV2, const XMVECTOR& triV3);

	// Solves the quadratic eqation, and returns the lowest root if equation is solvable, returns false if not solvable
	bool getLowestRoot(float a, float b, float c, float maxR, float* root);
};

#endif