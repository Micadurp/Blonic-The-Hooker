#ifndef DEFERREDRENDERING_H
#define DEFERREDRENDERING_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "Structs\Vertex.h"

class DeferredRendering
{
private:
	ID3D11Buffer* meshVertBuff;
	ID3D11Buffer* indexBuffer;

	int nrOfRenderTargets;
	UINT32 vertexSize;

	ID3D11RenderTargetView** renderTargetView;
	ID3D11ShaderResourceView** shaderResourceView;

	ID3D11ShaderResourceView ** clearRV;

	struct VertexCB
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 viewProjection;
	}
	vertexCB;

	ID3D11PixelShader * deferredPixelShader;

	ID3D11Buffer*   deferredCB; //Deferred vertex shader constant buffer


public:
	DeferredRendering();
	~DeferredRendering();

	void Initilize(ID3D11Device* _device, const DirectX::XMMATRIX &_projectionMatrix, int _screenWidth, int _screenHeight);
	void Shutdown();

	void FirstPass(ID3D11DeviceContext *deviceContext, ID3D11DepthStencilView* depthStencilView);
	bool SetShaders(ID3D11DeviceContext* _deviceContext);
	void Render(ID3D11DeviceContext *deviceContext, ID3D11DepthStencilView* depthStencilView, ID3D11RenderTargetView* gBackbufferRTV);
};
#endif