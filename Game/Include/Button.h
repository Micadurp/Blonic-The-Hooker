#ifndef BUTTON_H
#define BUTTON_H

#include "Model.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

using namespace std;

class Button: public Model
{
private:
	public:
		Button();
		virtual ~Button();

		bool Initialize(ID3D11Device* _device, wstring _modelName, const XMMATRIX& _newMatrix);

};

#endif