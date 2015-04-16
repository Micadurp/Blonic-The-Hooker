#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include <d3d11.h>
#include <d3dcompiler.h>

class RenderManager
{
private:
	ID3D11InputLayout* basicModelVertexLayout = nullptr;

	ID3D11VertexShader* basicModelVertexShader = nullptr;
	ID3D11GeometryShader* basicModelGeometryShader = nullptr;
	ID3D11PixelShader* basicModelPixelShader = nullptr;

	ID3D11Buffer*   basicModelVSCB = NULL; // Basic model vertex shader constant buffer
	ID3D11Buffer*   basicModelGSCB = NULL; // Basic model pixel shader constant buffer
	ID3D11Buffer*   basicModelPSCB = NULL; // Basic model geometry shader constant buffer


	ID3D11DepthStencilView* depthStencilView;



public:
	RenderManager();
	~RenderManager();

	bool SameShader();
};
#endif