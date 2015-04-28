#ifndef BUTTON_H
#define BUTTON_H

#include "Model.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

using namespace std;

class Button: Model
{
	private:
		int id;

	public:
		Button();
		virtual ~Button();

		bool Initialize(ID3D11Device* _device, int _id, wstring _modelName, vector<XMFLOAT3> *collidableGeometryPositions, vector<DWORD> *collidableGeometryIndices, const XMMATRIX& _newMatrix);

		int GetId() const;

		bool IntersectBox();
};

#endif