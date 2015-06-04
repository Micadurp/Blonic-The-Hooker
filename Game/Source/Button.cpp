#include"Button.h"

Button::Button()
{

}

Button::~Button()
{

}

bool Button::Initialize(ID3D11Device* _device, wstring _modelName, const XMMATRIX& _newMatrix)
{
	bool result;

	result = Model::Initialize(_modelName, _device);
	if (!result)
	{
		return false;
	}
	Model::SetObjMatrix(_newMatrix);

	return true;
}
