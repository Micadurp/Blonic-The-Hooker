#ifndef COLLISIONPACKET_H
#define COLLISIONPACKET_H

#include <DirectXMath.h>

struct CollisionPacket {
	// Information about ellipsoid (in world space)
	DirectX::XMVECTOR ellipsoidSpace;
	DirectX::XMVECTOR w_Position;
	DirectX::XMVECTOR w_Velocity;

	// Information about ellipsoid (in ellipsoid space)
	DirectX::XMVECTOR e_Position;
	DirectX::XMVECTOR e_Velocity;
	DirectX::XMVECTOR e_normalizedVelocity;

	// Collision Information
	bool foundCollision;
	float nearestDistance;
	DirectX::XMVECTOR intersectionPoint;
	int collisionRecursionDepth;
};

#endif