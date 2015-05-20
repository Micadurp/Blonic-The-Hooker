#ifndef SKYBOX_H
#define SKYBOX_H

#include"Model.h"

class SkyBox: public Model
{
private:
	ID3D11VertexShader * skyVertexShader;
	ID3D11PixelShader * skyPixelShader;

	ID3D11DepthStencilState* depthStencilState;

public:
	SkyBox();
	~SkyBox();

	bool Initialize(std::wstring _modelName, ID3D11Device* _device);
	void Update(Camera*);
	void Shutdown();

	void Render(ID3D11DeviceContext* _deviceContext, ID3D11DepthStencilState * _depthState);
};
#endif