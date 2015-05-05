#ifndef DEFERREDRENDERING_H
#define DEFERREDRENDERING_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>


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

	struct Vertex
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT2 texture;

		Vertex()
		{
			position.x = 0;
			position.y = 0;
			position.z = 0;
			position.w = 0;

			texture.x = 0;
			texture.y = 0;

		}
		Vertex(float _x, float _y, float _z, float _U, float _V)
		{
			position.x = _x;
			position.y = _y;
			position.z = _z;
			position.w = 0;

			texture.x = _U;
			texture.y = _V;


		}
	};

	struct VertexCB
	{
		DirectX::XMMATRIX projectionMatrix;
	}
	vertexCB;

	ID3D11VertexShader * deferredVertexShader;
	ID3D11PixelShader * deferredPixelShader;

	ID3D11Buffer*   deferredCB; //Deferred vertex shader constant buffer

	ID3D11InputLayout* deferredVertexLayout = nullptr;

public:
	DeferredRendering();
	~DeferredRendering();

	void Initilize(ID3D11Device* _device, const DirectX::XMMATRIX &_projectionMatrix, int _screenWidth, int _screenHeight);

	void FirstPass(ID3D11DeviceContext *deviceContext, ID3D11DepthStencilView* depthStencilView);
	void Render(ID3D11DeviceContext *deviceContext, ID3D11DepthStencilView* depthStencilView, ID3D11RenderTargetView* gBackbufferRTV);
};
#endif