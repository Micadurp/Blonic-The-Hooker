#include"Button.h"

Button::Button()
{

}

Button::~Button()
{

}

bool Button::Initialize(ID3D11Device* _device, int _id, wstring _modelName, vector<XMFLOAT3> *collidableGeometryPositions, vector<DWORD> *collidableGeometryIndices, const XMMATRIX& _newMatrix)
{
	bool result;

	id = _id;

	Model::SetObjMatrix(_newMatrix);
	result = Model::Initialize(_modelName, _device, collidableGeometryPositions, collidableGeometryIndices);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Button::Initialize(ID3D11Device* _device, int _id, wstring _modelName, const XMMATRIX& _newMatrix)
{
	bool result;

	id = _id;

	Model::SetObjMatrix(_newMatrix);
	result = Model::Initialize(_modelName, _device);
	if (!result)
	{
		return false;
	}

	return true;
}

int Button::GetId() const
{
	return id;
}