#include"Button.h"

Button::Button()
{

}

Button::~Button()
{

}

bool Button::Initialize(ID3D11Device* _device, int _id, wstring _modelName, vector<XMFLOAT3> *collidableGeometryPositions, vector<DWORD> *collidableGeometryIndices, const XMMATRIX& _newMatrix)
{
	id = _id;

	Model::SetObjMatrix(_newMatrix);
	Model::Initialize(_modelName, _device, collidableGeometryPositions, collidableGeometryIndices)
}

bool Button::IntersectBox()
{
	return false;
}