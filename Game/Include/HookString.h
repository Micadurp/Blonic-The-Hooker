#ifndef HOOKSTRING_H
#define HOOKSTRING_H

#include<vector>


#include"Model.h"
#include"Direct3D.h"


class HookString:public Model
{
private:
	bool isActive;
	XMVECTOR * point;

	struct GeometryShaderBuffer
	{
		XMVECTOR playerPos;
		XMVECTOR targetPos;
		int intNrOfParticles;
	};

	ID3D11Buffer*   hookStringGSCB; // Basic model geometry shader constant buffer
public:
	HookString();
	~HookString();

	bool Initialize(ID3D11Device *);

	void Update(ID3D11DeviceContext* _deviceContext, XMVECTOR * _playerPos, XMVECTOR * _target);

	void Render(Direct3D* _direct3D);

	bool GetActive();

	void SetActive(bool _isActive);
};
#endif