#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

class RenderManager
{
private:

	struct VertexShaderBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX viewProjection;
	};

	ID3D11InputLayout* basicModelVertexLayout = nullptr;

	ID3D11VertexShader* basicModelVertexShader = nullptr;
	ID3D11GeometryShader* basicModelGeometryShader = nullptr;
	ID3D11PixelShader* basicModelPixelShader = nullptr;

	ID3D11Buffer*   basicModelVSCB; // Basic model vertex shader constant buffer
	ID3D11Buffer*   basicModelGSCB; // Basic model pixel shader constant buffer
	ID3D11Buffer*   basicModelPSCB; // Basic model geometry shader constant buffer


	ID3D11DepthStencilView* depthStencilView;



public:
	RenderManager();
	~RenderManager();

	bool Initilize(ID3D11Device* _device);

	void Shutdown();
	bool SameShader();

	bool SetShader(ID3D11DeviceContext* _deviceContext, const DirectX::XMMATRIX &_worldMatrix, const DirectX::XMMATRIX &_viewMatrix, const DirectX::XMMATRIX &_projectionMatrix);
};
#endif